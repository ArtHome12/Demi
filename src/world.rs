/* ===============================================================================
Simulation of the evolution of the animal world.
The world as a set of dots.
17 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2022 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::sync::{Arc, atomic::{Ordering, AtomicBool, AtomicUsize,}, Mutex, };
use std::time::{Duration, Instant, };

use crate::{dot::Sheet, evolution::Evolution, environment::*};
pub use crate::dot::{Dot, Sheets, PtrSheets};
use crate::geom::*;
use crate::project::{Project, Element, };
use crate::organism::*;
use crate::reactions::UIReactions;

pub struct World {
   run_flag: Arc<AtomicBool>, // requirement for thread run if true else pause
   state_flag: Arc<AtomicBool>, // actual state of thread
   ticks_elapsed: Arc<AtomicUsize>, // model time - a number ticks elapsed from beginning
   elements_sheets: PtrSheets,
   animal_sheet: Arc<Mutex<AnimalSheet>>, // Mirror from Evaluation
   size: Size,

   // Section for UI
   pub vis_elem_indexes: Vec<bool>, // indexes of visible (non-filtered) elements
   pub vis_reac_indexes: Vec<bool>, // indexes of visible (non-filtered) reactions
   pub vis_dead: bool,
   pub ui_reactions: UIReactions,
   // pub elements_count: usize,
   pub elements: Vec<Element>,
}

impl World {
   pub fn new(project: Project) -> Self {
      // Initialize structures from project
      let elements = project.elements;
      let reactions = project.reactions;
      let ui_reactions = project.ui_reactions;
      let size = project.size;

      // Create sheets with initial amounts
      let sheets = elements.iter().map(|v| {
         Sheet::new(size, v.init_amount, v.volatility)
      }).collect::<Sheets>();

      let env = Environment::new(size, project.resolution, project.luca_reaction);

      // Create animals
      let animal_sheet = AnimalSheet::new(size);
      let animal_sheet = Arc::new(Mutex::new(animal_sheet));

      // Evolution algorithm
      let mut evolution = Evolution::new(sheets, Arc::clone(&animal_sheet), reactions);

      // Store raw pointers to elements
      let elements_sheets = PtrSheets::create(&evolution.sheets);

      // Flags for thread control
      let run_flag = Arc::new(AtomicBool::new(false));
      let run_flag_threaded = Arc::clone(&run_flag);
      let state_flag = Arc::new(AtomicBool::new(false));
      let state_flag_threaded = Arc::clone(&state_flag);

      // The model time
      let ticks_elapsed = Arc::new(AtomicUsize::new(0));
      let ticks_elapsed_threaded = Arc::clone(&ticks_elapsed);

      // Thread for calculate evolution
      tokio::task::spawn_blocking(move || {
         let sleep_time = Duration::from_millis(100);
         
         // Running until program not closed
         loop {

            // Sleep if it paused
            if run_flag_threaded.load(Ordering::Acquire) {
               // Transfer signal to parent
               state_flag_threaded.store(true, Ordering::Release);

               // Increase model time
               let tick = ticks_elapsed_threaded.fetch_add(1, Ordering::Relaxed);

               // Calculate the tick of evolution
               evolution.make_tick(&env, tick);
            } else {
               state_flag_threaded.store(false, Ordering::Release);
               std::thread::sleep(sleep_time);
            }
         }
      });

      // At start all elements should be visible, collect its indexes
      let len = elements.len();
      let vis_elem_indexes = vec![true; len];
      let vis_reac_indexes = vec![true; len];

      Self {
         run_flag,
         state_flag,
         ticks_elapsed,
         elements_sheets,
         animal_sheet,
         size,

         vis_elem_indexes,
         vis_reac_indexes,
         vis_dead: true,
         ui_reactions,
         elements,
      }
   }

   // Return dot at display position
   // The world must be continuous, the first point goes to the right (or bottom) of the last point again
   pub fn dot(&self, display_x: isize, display_y: isize) -> Dot {
      let Size {x: width, y: height} = self.size();
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
      let serial_bit = self.size.serial(x, y);
      let energy = self.elements_sheets.get(0, serial_bit);

      // Find the dot color among animals
      let unlocked_sheet =self.animal_sheet.lock().unwrap();
      let stack = unlocked_sheet.get(serial_bit);

      // Among animals determines with visible reaction and alive or not
      let animal_color = stack.iter()
      .find_map(|o| {
         // Need to be visible and alive or not
         let reaction = o.reaction_index();
         let visible = self.vis_reac_indexes[reaction];
         if visible && (self.vis_dead || o.alive()) {
            let reaction = self.ui_reactions.get(reaction);
            Some(reaction.color)
         } else {
            None
         }
      });

      let mut color = if let Some(color) = animal_color {
         color
      } else {
         // Among elements color determines the element with non-zero amount among visible (non-filtered)
         let color = self.vis_elem_indexes.iter()
         .enumerate()
         .find_map(|(item_index, visible)| {
            if *visible && self.elements_sheets.get(item_index, serial_bit) > 0 {
               Some(self.elements[item_index].color)
            } else {
               None
            }
         });
         
         if let Some(color) = color {
            color
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
      let serial_bit = self.size.serial(dot.x, dot.y);

      // Current time
      let now = self.ticks_elapsed();

      let mut remaining_lines = max_lines;

      // Collect info among animals
      let unlocked_sheet = self.animal_sheet.lock().unwrap();
      let stack = unlocked_sheet.get(serial_bit);

      // Among animals determines with visible reaction and alive or not
      let filtered_animals = stack.iter()
      .filter(|o| {
         // Need to be visible and alive or not
         let reaction = o.reaction_index();
         let visible = self.vis_reac_indexes[reaction];
         visible && (self.vis_dead || o.alive())
      });

      // Animal world
      let animal_desc = filtered_animals
      .take(max_lines)
      .fold(String::default(), |acc, o| {
         // After death, the date of birth contains the age at death
         let age = if o.alive() { now.saturating_sub(o.birthday) } else { o.birthday };
         
         // Decrease max lines (side effect)
         remaining_lines -= 1;

         format!("{}[{}۩ {}]{}", acc, age, o, delimiter)
      });

      // Inanimal world
      self.vis_elem_indexes.iter()
      .enumerate()
      .filter(|(_index, vis)| **vis)
      .take(remaining_lines)
      .fold(animal_desc, |acc, (vis_index, _)| {
         format!("{}{}: {}{}", acc, self.elements[vis_index].name, self.elements_sheets.get(vis_index, serial_bit), delimiter)
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
      self.size
   }

   pub fn date(&self) -> (usize, usize) {
      let now = self.ticks_elapsed();
      Environment::date(now)
   }

   fn await_for_stop_thread_or_panic(&self) {
      let timeout = Duration::from_secs(5);
      let sleep_time = Duration::from_millis(100);
      let now = Instant::now();

      while now.elapsed() < timeout {
         let run = self.state_flag.load(Ordering::Acquire);
         if !run {
            return;
         }
         std::thread::sleep(sleep_time);
      }
      panic!("await_for_stop_thread too long")
   }


   pub fn save(&self) {
      // Stop evaluate if running
      let prev_state = self.run_flag.load(Ordering::Acquire);
      if prev_state {
         self.toggle_run();
      }

      // Await for stop
      self.await_for_stop_thread_or_panic();

      // Resume evaluation if nesessary
      if prev_state {
         self.toggle_run();
      }
   }
}
