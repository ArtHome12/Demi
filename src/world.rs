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
use iced::Color;
use std::{thread::{self, JoinHandle}};
use std::sync::{Arc, Mutex, atomic::{Ordering, AtomicBool, AtomicUsize,}};

use crate::{dot::{Bit, Bits,}, evolution::Evolution, };
pub use crate::dot::{Dot, Coord,};
use crate::project;

pub struct World {
   project: project::Project,
   mutex_bits: Arc<Mutex<Bits>>,
   thread: JoinHandle<()>, // thread for calculate evolution
   run_flag: Arc<AtomicBool>, // running when true else paused
   ticks_elapsed: Arc<AtomicUsize>, // model time - a number ticks elapsed from beginning
}

impl World {
   pub fn new(project: project::Project) -> Self {
      let width = project.width;
      let height= project.height;
      let elements_amount = project.elements_amount();

      // Create vec for rows
      let mut bits = Vec::with_capacity(height);

      for x in 0..width {
         // Create vec for items
         let row = (0..height).into_iter().map(|y| Bit::new(x, y, elements_amount.clone())).collect();

         // Put items into result vector
         bits.push(row);
      };
      let mutex_bits = Arc::new(Mutex::new(bits));

      // Evolution algorithm
      let mut evolution = Evolution::new(Arc::clone(&mutex_bits), Coord::new(width, height));

      // Flags for thread control
      let run_flag = Arc::new(AtomicBool::new(false));
      let run_flag_threaded = Arc::clone(&run_flag);

      // The model time
      let ticks_elapsed = Arc::new(AtomicUsize::new(0));
      let ticks_elapsed_threaded = Arc::clone(&ticks_elapsed);

      // Thread for calculate evolution
      let thread = thread::spawn(move || {
         // Running until program not closed
         loop {

            // Sleep if it paused
            if !run_flag_threaded.load(Ordering::Acquire) {
               thread::park();
            }

            // Increase model time
            let tick = ticks_elapsed_threaded.fetch_add(1, Ordering::Relaxed);

            // Calculate the tick of evolution
            evolution.make_tick(tick);
         }
      });

      Self {
         project,
         mutex_bits,
         // evolution,
         thread,
         run_flag,
         ticks_elapsed,
      }
   }

   // Return dot at display position
   // The world must be continuous, the first point goes to the right (or bottom) of the last point again
   pub fn dot(&self, display_x: isize, display_y: isize) -> Dot {
      let width = self.project.width as isize;
      let height = self.project.height as isize;
      let mut x = display_x;
      let mut y = display_y;

      while x < 0 {x += width;}
      while x >= width {x -= width;}
      while y < 0 {y += height;}
      while y >= height {y -= height;}

      let x = x as usize;
      let y = y as usize;

      // Corresponding bit of the world
      let bit = &self.mutex_bits.lock().unwrap()[x][y];
      let color = if bit.amount(0) != 0.0 {self.project.elements[0].color} else {Color::TRANSPARENT};

      Dot{x, y, color,}
   }

   // Return the corresponding bit for the point
   /*fn bit(&self, dot: &Dot) -> &Bit {
      &self.mutex_bits.lock().unwrap()[dot.x][dot.y]
   }*/

   // Text to describe a point with a size constraint
   pub fn description(&self, dot: &Dot, max_lines: usize, delimiter: char) -> String {
      // Underlying bit for dot
      let bit = &self.mutex_bits.lock().unwrap()[dot.x][dot.y];

      self.project.elements.iter()
      .take(max_lines - 1) // -1 for energy
      .enumerate()
      .fold(format!("Energy: {}%", bit.energy), |acc, (i, element)| {
         format!("{}{}{}: {}", acc, delimiter, element.name, bit.elements[i])
      })
   }

   // Temporary for testing
   pub fn populate(&mut self, dot: &Dot) {
      let bit = &mut self.mutex_bits.lock().unwrap()[dot.x][dot.y];
      bit.set_amount(0, 1.0);
   }
   pub fn unpopulate(&mut self, dot: &Dot) {
      let bit = &mut self.mutex_bits.lock().unwrap()[dot.x][dot.y];
      bit.set_amount(0, 0.0);
   }

   pub fn width(&self) -> usize {
      self.project.width
   }

   pub fn height(&self) -> usize {
      self.project.height
   }

   // Pause/resume evolutuon thread
   pub fn toggle_pause(&self) {
      // Transfer signal to thread
      let flag = !self.run_flag.load(Ordering::Acquire);
      self.run_flag.store(flag, Ordering::Release);

      // If it has been suspended, wake up
      if flag {self.thread.thread().unpark()}
   }

   // Returns model time - a number ticks elapsed from beginning
   pub fn ticks_elapsed(&self) -> usize {
      self.ticks_elapsed.load(Ordering::Relaxed)
   }
}


