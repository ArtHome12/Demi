/* ===============================================================================
Simulation of the evolution of the animal world.
The world as a set of dots.
17 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::{rc::Rc, cell::RefCell, };
use std::sync::{Arc, Mutex, atomic::{Ordering, AtomicBool, AtomicUsize,}};
use std::time::Duration;

use crate::{dot::{Sheets, Sheet, }, evolution::Evolution, environment::*};
pub use crate::dot::{Dot, };
use crate::geom::*;
use crate::project;

pub struct World {
   pub project: Rc<RefCell<project::Project>>,
   mutex_sheets: Arc<Mutex<Sheets>>,
   run_flag: Arc<AtomicBool>, // running when true else paused
   ticks_elapsed: Arc<AtomicUsize>, // model time - a number ticks elapsed from beginning
   env: Environment,
}

impl World {
   pub fn new(project: Rc<RefCell<project::Project>>) -> Self {

      let project_cloned = project.clone();
      let pr = project_cloned.borrow();

      let env = Environment::new(&pr);

      // Create sheets with initial amounts
      let mut sheets = pr.elements.iter().map(|v| {
         Sheet::new(pr.size, v.init_amount, v.volatility)
      }).collect::<Vec<Sheet>>();

      // Energy first
      let solar = Sheet::new(pr.size, 0, 0.0);
      sheets.insert(0, solar);

      let mutex_sheets = Arc::new(Mutex::new(sheets));

      // Evolution algorithm
      let mut evolution = Evolution::new(Arc::clone(&mutex_sheets));

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
         mutex_sheets,
         run_flag,
         ticks_elapsed,
         env,
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
      let locked_sheets = self.mutex_sheets.lock().unwrap();
      let energy = locked_sheets[0].get(serial_bit);

      // The dot color determines the element with non-zero amount among visible (non-filtered)
      let color_index = pr.vis_elem_indexes.iter().find(|i| {
         // sheet with [0] for energy
         let amount = locked_sheets[**i + 1].get(serial_bit);
         amount > 0
      });
      let mut color = if let Some(color_index) = color_index {
         pr.elements[*color_index].color
      } else {iced::Color::BLACK};

      // Adjust color to energy
      color.a = energy as f32 / 100.0;

      Dot{x, y, color,}
   }

   // Text to describe a point with a size constraint
   pub fn description(&self, dot: &Dot, max_lines: usize, delimiter: char) -> String {
      // Underlying bit serial number for dot
      let serial_bit = self.env.serial(dot.x, dot.y);

      // Sheets with amounts
      let locked_sheets = self.mutex_sheets.lock().unwrap();

      // For visible element names
      let pr = self.project.borrow();

      pr.vis_elem_indexes.iter()
      .take(max_lines - 1) // -1 for energy
      .enumerate()
      .fold(format!("Energy: {}%", locked_sheets[0].get(serial_bit)), |acc, (i, element_i)| {
         format!("{}{}{}: {}", acc, delimiter, pr.elements[*element_i].name, locked_sheets[i + 1].get(serial_bit))
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


