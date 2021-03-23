/* ===============================================================================
Simulation of the evolution of the animal world.
Mechanism of evolution for the world.
03 Mar 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::sync::{Arc, Mutex, };

use crate::{dot::{/*Bit,*/ Bits, Coord,}};

pub struct Evolution {
   // World size
   width: usize,
   height: usize,

   pub bits: Bits, // points of the world with data for evaluate
   mirror: Arc<Mutex<Bits>>, // for display

	// The maximum illumination radius around the location of the Sun is 90% of the height of the world
	light_radius: usize,

   // The course of the Sun vertically, due to the inclination of the earth axis, approximately sin (23.5) = 0.4 or 10% on each side of the equator
	tropic_height: f32,
}

impl Evolution {
   // Number of ticks per day and days per year
   const TICKS_PER_DAY: f32 = 60.0*24.0;
   const DAYS_PER_YEAR: f32 = Self::TICKS_PER_DAY * 365.0;
   const HALF_YEAR: f32 = Self::DAYS_PER_YEAR / 2.0 + 0.5;


   pub fn new(mirror: Arc<Mutex<Bits>>, size: &Coord) -> Self {
      // Initial value from saves or project
      let raw_bits = {mirror.lock().unwrap().clone()};

      Self {
         width: size.x,
         height: size.y,
         bits: raw_bits,
         mirror,
         light_radius: (0.8 * size.y as f32 / 2.0) as usize,
         tropic_height: size.y as f32 / 5.0
      }
   }

   pub fn make_tick(&mut self, tick: usize) {
      // Sun position
      let sun_pos = self.sun_position(tick);

      let w = self.width as isize;
      let r = self.light_radius as isize;
      let rf = r as f64;

      // Sun radiation
      for x in -r..=r {
         // From sun position to bit index with correction of the border
         let mut i = x + sun_pos.x as isize;
         if i < 0 {i += w}
         else if i >= w {i -= w}

         for y in -r..=r {
            // Vertical index does not require correction
            let j = y + sun_pos.y as isize;

            // Distance from the center of the sun
            let d = ((x * x + y * y) as f64).sqrt();

            // Brightness is inversely proportional to distance
            let b = if d < rf {((rf - d) / rf * 100.0).round()} else {0.0};

            // Update energy in the bit
            let coord = Coord{x: i as usize, y: j as usize};
            self.bits.set_energy(&coord, b as usize);
         }
      }

      // Transfer data to mirror if there no delay
      if let Ok(ref mut mirror_bits) = self.mirror.try_lock() {
         mirror_bits.clone_from_slice(self.bits.as_slice())
      }
   }

   // Returns the position of the sun at the specified time
   fn sun_position(&self, tick: usize) -> Coord {
      // Times of Day
      let day_tick = tick as f32 % Self::TICKS_PER_DAY;

      let w = self.width as f32;
      let h = self.height as f32;

      // The sun moves from east to west in proportion to the elapsed fraction of a day
      let x = w * (1.0 - day_tick / (Self::TICKS_PER_DAY - 1.0) + 0.5);

      // When the first half of the year goes, it is necessary to take a share from the equator, and when the second is to add
      let day = tick as f32 % Self::DAYS_PER_YEAR;
      let y = if day < Self::HALF_YEAR {
		   (h - self.tropic_height) / 2.0 + day * self.tropic_height / Self::HALF_YEAR
      } else {
		   (h + self.tropic_height) / 2.0 - (day - Self::HALF_YEAR) * self.tropic_height / Self::HALF_YEAR
      };

      Coord::new(x as usize, y as usize)
   }
}
