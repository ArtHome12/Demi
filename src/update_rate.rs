/* ===============================================================================
Simulation of the evolution of the animal world.
Speed calculation (FPS, model time).
07 Mar 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#[derive(Default)]
pub struct UpdateRate {
   rate: usize, // Last computed value
   ticks: usize,  // Number of ticks since the beginning of a second
}


impl UpdateRate {
   // Update counter
   pub fn make_tick(&mut self) {
      self.ticks += 1
   }

   // Start new second
   pub fn clock_chime(&mut self) {
      self.rate = self.ticks;
      self.ticks = 0;
   }
}