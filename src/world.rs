/* ===============================================================================
Simulation of the evolution of the animal world.
The world as a set of dots.
17 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2022 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::{rc::Rc, cell::RefCell, ptr, };
use std::sync::{Arc, atomic::{Ordering, AtomicBool, AtomicUsize,}, Mutex, };
use std::time::Duration;

use crate::{dot::Sheet, evolution::Evolution, environment::*};
pub use crate::dot::{Dot, Sheets, };
use crate::geom::*;
use crate::project;
use crate::organism::*;

pub struct World {
   pub project: Rc<RefCell<project::Project>>,
   run_flag: Arc<AtomicBool>, // running when true else paused
   ticks_elapsed: Arc<AtomicUsize>, // model time - a number ticks elapsed from beginning
   env: Environment,
   elements_sheets: Vec<*const usize>,
   animal_sheet: Arc<Mutex<AnimalSheet>>, // Mirror from Evaluation
}

impl World {
   pub fn new(project: Rc<RefCell<project::Project>>) -> Self {

      let project_cloned = project.clone();
      let pr = project_cloned.borrow();

      let env = Environment::new(&pr);

      // Create sheets with initial amounts
      let sheets = pr.elements.iter().map(|v| {
         Sheet::new(pr.size, v.init_amount, v.volatility)
      }).collect::<Sheets>();

      // Create animals
      let animal_sheet = AnimalSheet::new(pr.size);
      let animal_sheet = Arc::new(Mutex::new(animal_sheet));

      // Evolution algorithm
      let mut evolution = Evolution::new(sheets, Arc::clone(&animal_sheet));

      // Store raw pointers to elements
      let elements_sheets = evolution.sheets.get().iter()
      .map(|sheet| ptr::addr_of!(sheet.matrix[0]))
      .collect();

      // Flags for thread control
      let run_flag = Arc::new(AtomicBool::new(false));
      let run_flag_threaded = Arc::clone(&run_flag);

      // The model time
      let ticks_elapsed = Arc::new(AtomicUsize::new(0));
      let ticks_elapsed_threaded = Arc::clone(&ticks_elapsed);

      // Thread for calculate evolution
      let cloned_env = env.clone();
      tokio::task::spawn_blocking(move || {
         // Running until program not closed
         loop {

            // Sleep if it paused
            if run_flag_threaded.load(Ordering::Acquire) {
               // Increase model time
               let tick = ticks_elapsed_threaded.fetch_add(1, Ordering::Relaxed);

               // Calculate the tick of evolution
               evolution.make_tick(&cloned_env, tick);
            } else {
               std::thread::sleep(Duration::from_millis(100));
            }
         }
      });

      Self {
         project,
         run_flag,
         ticks_elapsed,
         env,
         elements_sheets,
         animal_sheet,
      }
   }

   // Return dot at display position
   // The world must be continuous, the first point goes to the right (or bottom) of the last point again
   pub fn dot(&self, display_x: isize, display_y: isize) -> Dot {
      let pr = self.project.borrow();
      let Size {x: width, y: height} = pr.size;
      let width = width as isize;
      let height = height as isize;

      let mut x = display_x;
      let mut y = display_y;

      while x < 0 {x += width;}
      while x >= width {x -= width;}
      while y < 0 {y += height;}
      while y >= height {y -= height;}

      let x = x as usize;
      let y = y as usize;

      // Corresponding bit of the world
      let serial_bit = self.env.serial(x, y);
      let energy = unsafe{ self.elements_sheets[0].add(serial_bit).read() };

      // Find the dot color among animals
      let unlocked_sheet =self.animal_sheet.lock().unwrap();
      let animals = unlocked_sheet.get(serial_bit);
      let first_alive = animals.iter().find(|o| o.alive());

      let mut color = if let Some(organism) = first_alive {
         organism.color()
      } else {
         // Among elements color determines the element with non-zero amount among visible (non-filtered)
         let color_index = pr.vis_elem_indexes.iter().find(|i| {
            // sheet with [0] for energy
            let amount = unsafe{ self.elements_sheets[**i + 1].add(serial_bit).read() };
            amount > 0
         });
         
         if let Some(color_index) = color_index {
            pr.elements[*color_index].color
         } else {
            iced::Color::BLACK
         }
      };

      // Adjust color to energy
      color.a = energy as f32 / 100.0;

      Dot{x, y, color,}
   }

   // Text to describe a point with a size constraint
   pub fn description(&self, dot: &Dot, max_lines: usize, delimiter: char) -> String {
      // Underlying bit serial number for dot
      let serial_bit = self.env.serial(dot.x, dot.y);

      // Current time
      let now = self.ticks_elapsed();

      let mut remaining_lines = max_lines;

      // Animal world
      let animal_desc = if let Ok(animals) = self.animal_sheet.lock() {
         let stack = animals.get(serial_bit); // Animals at the point

         // Decrease max lines
         remaining_lines -= stack.len();

         stack.iter()
         .take(max_lines)
         .fold(String::default(), |acc, o| {
            // After death, the date of birth contains the age at death
            let age = if o.alive() { now.saturating_sub(o.birthday) } else { o.birthday };
            
            format!("{}[{}۩ {}]{}", acc, age, o, delimiter)
         })
      } else {
         String::default()
      };

      // For visible element names
      let pr = self.project.borrow();

      pr.vis_elem_indexes.iter()
      .take(remaining_lines)
      .enumerate()
      .fold(animal_desc, |acc, (i, element_i)| {
         format!("{}{}: {}{}", acc, pr.elements[*element_i].name, unsafe{ self.elements_sheets[i].add(serial_bit).read() }, delimiter)
      })
   }

   // Pause/resume evolutuon thread
   pub fn toggle_run(&self) {
      // Transfer signal to thread
      let flag = !self.run_flag.load(Ordering::Acquire);
      self.run_flag.store(flag, Ordering::Release);
   }

   // Returns model time - a number ticks elapsed from beginning
   pub fn ticks_elapsed(&self) -> usize {
      self.ticks_elapsed.load(Ordering::Relaxed)
   }

   pub fn size(&self) -> Size {
      self.project.borrow().size
   }
}


