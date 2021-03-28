/* ===============================================================================
Simulation of the evolution of the animal world.
The world as a set of dots.
17 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::usize;
// use std::{thread::{self, JoinHandle}};
use std::sync::{Arc, Mutex, atomic::{Ordering, AtomicBool, AtomicUsize,}};
use std::time::Duration;

use crate::{dot::{Sheets, Sheet, }, evolution::Evolution, environment::*};
pub use crate::dot::{Dot, };
use crate::geom::*;
use crate::project;

pub struct World {
   project: project::Project,
   mutex_sheets: Arc<Mutex<Sheets>>,
   // thread: tokio::task::JoinHandle<()>, // thread for calculate evolution
   run_flag: Arc<AtomicBool>, // running when true else paused
   ticks_elapsed: Arc<AtomicUsize>, // model time - a number ticks elapsed from beginning
   env: Environment,
}

impl World {
   pub fn new(project: project::Project) -> Self {

      let env = Environment::new(&project);

      // Create sheets with initial amounts
      let mut sheets = project.elements.iter().map(|v| {
         Sheet::new(project.size, v.amount, v.volatility)
      }).collect::<Vec<Sheet>>();

      // Energy first
      let solar = Sheet::new(project.size, 0, 0.0);
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
      tokio::task::spawn_blocking(move || {
         // Running until program not closed
         loop {

            // Sleep if it paused
            if run_flag_threaded.load(Ordering::Acquire) {
               // Increase model time
               let tick = ticks_elapsed_threaded.fetch_add(1, Ordering::Relaxed);

               // Calculate the tick of evolution
               evolution.make_tick(&env, tick);

            // thread::park();
            } else {
               std::thread::sleep(Duration::from_millis(100));
            }
         }
      });

      Self {
         project,
         mutex_sheets,
         // evolution,
         // thread,
         run_flag,
         ticks_elapsed,
         env,
      }
   }

   // Return dot at display position
   // The world must be continuous, the first point goes to the right (or bottom) of the last point again
   pub fn dot(&self, display_x: isize, display_y: isize) -> Dot {
      let Size {x: width, y: height} = self.project.size;
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

      // The dot color determines the element with non-zero amount
      let mut non_zero_index = 1usize;
      for i in 1..locked_sheets.len() {
         // Relative concentration
         let amount = locked_sheets[i].get(serial_bit);
         if amount > 0 {
            non_zero_index = i;
            break;
         }
      }
      let mut color = self.project.elements[non_zero_index - 1].color;

      // Adjust color to energy
      color.a = energy as f32 / 100.0;

      Dot{x, y, color,}
   }

   // Text to describe a point with a size constraint
   pub fn description(&self, dot: &Dot, max_lines: usize, delimiter: char) -> String {
      // Underlying bit serial number for dot
      let serial_bit = self.env.serial(dot.x, dot.y);

      let locked_sheets = self.mutex_sheets.lock().unwrap();

      self.project.elements.iter()
      .take(max_lines - 1) // -1 for energy
      .enumerate()
      .fold(format!("Energy: {}%", locked_sheets[0].get(serial_bit)), |acc, (i, element)| {
         format!("{}{}{}: {}", acc, delimiter, element.name, locked_sheets[i + 1].get(serial_bit))
      })
   }

   // Temporary for testing
   pub fn populate(&mut self, dot: &Dot) {
      let serial_bit = self.env.serial(dot.x, dot.y);
      let locked_sheets = &mut self.mutex_sheets.lock().unwrap();
      locked_sheets[1].set(serial_bit, 1);
   }
   pub fn unpopulate(&mut self, dot: &Dot) {
      let serial_bit = self.env.serial(dot.x, dot.y);
      let locked_sheets = &mut self.mutex_sheets.lock().unwrap();
      locked_sheets[1].set(serial_bit, 0);
   }

   // Pause/resume evolutuon thread
   pub fn toggle_pause(&self) {
      // Transfer signal to thread
      let flag = !self.run_flag.load(Ordering::Acquire);
      self.run_flag.store(flag, Ordering::Release);

      // If it has been suspended, wake up
      // if flag {self.thread.thread().unpark()}
   }

   // Returns model time - a number ticks elapsed from beginning
   pub fn ticks_elapsed(&self) -> usize {
      self.ticks_elapsed.load(Ordering::Relaxed)
   }

   pub fn size(&self) -> &Size {
      &self.project.size
   }
}


