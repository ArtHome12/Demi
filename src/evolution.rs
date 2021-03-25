/* ===============================================================================
Simulation of the evolution of the animal world.
Mechanism of evolution for the world.
03 Mar 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::{sync::{Arc, Mutex, }};
use rand::{Rng, prelude::{ThreadRng}};


use crate::dot::*;
use crate::geom::*;
use crate::project::Project;

pub struct Evolution {
   // World size
   world_size: Size,

   pub bits: Bits, // points of the world with data for evaluate
   mirror: Arc<Mutex<Bits>>, // for display

	// The maximum illumination radius around the location of the Sun is 90% of the height of the world
	light_radius: usize,

   // The course of the Sun vertically, due to the inclination of the earth axis, approximately sin (23.5) = 0.4 or 10% on each side of the equator
	tropic_height: f32,

   // Number of elements
   element_count: usize,

   // How many bits to move (diffusion) elements in one tick
   num_points_to_diffuse: usize,

   // Volatility of elements
   volatility: Vec<f32>,

   // Range for serial number of all bits
   bits_count: usize,
}

impl Evolution {
   // Number of ticks per day and days per year
   const TICKS_PER_DAY: f32 = 60.0*24.0;
   const DAYS_PER_YEAR: f32 = Self::TICKS_PER_DAY * 365.0;
   const HALF_YEAR: f32 = Self::DAYS_PER_YEAR / 2.0 + 0.5;


   pub fn new(mirror: Arc<Mutex<Bits>>, project: &Project) -> Self {
      // Initial value from saves or project
      let raw_bits = {mirror.lock().unwrap().clone()};
      let element_count = project.elements.len();
      let world_height = project.size.y;

      // How many bits to move (diffusion) elements in one tick
      let bits_count = project.size.x * world_height;
      let num_points_to_diffuse = (bits_count as f32 * project.resolution) as usize;

      let volatility = project.elements.iter().map(|e| e.volatility).collect();

      Self {
         world_size: project.size,
         bits: raw_bits,
         mirror,
         light_radius: (0.8 * world_height as f32 / 2.0) as usize,
         tropic_height: world_height as f32 / 5.0,
         element_count,
         num_points_to_diffuse,
         volatility,
         bits_count,
      }
   }

   pub fn make_tick(&mut self, tick: usize) {
      // Irradiate with solar energy
      self.shine(tick);

      // Shuffle elements
      let mut rng = rand::thread_rng();
      (0..self.element_count).for_each(|i| self.diffusion(i, self.volatility[i], &mut rng));

      // Transfer data to mirror if there no delay
      if let Ok(ref mut mirror_bits) = self.mirror.try_lock() {
         mirror_bits.clone_from_slice(self.bits.as_slice())
      }
   }

   // Returns the position of the sun at the specified time
   fn sun_position(&self, tick: usize) -> Coord {
      // Times of Day
      let day_tick = tick as f32 % Self::TICKS_PER_DAY;

      let Size{x: w, y: h} = self.world_size;

      // The sun moves from east to west in proportion to the elapsed fraction of a day
      let x = w as f32 * (1.0 - day_tick / (Self::TICKS_PER_DAY - 1.0) + 0.5);

      // When the first half of the year goes, it is necessary to take a share from the equator, and when the second is to add
      let day = tick as f32 % Self::DAYS_PER_YEAR;
      let y = if day < Self::HALF_YEAR {
		   (h as f32 - self.tropic_height) / 2.0 + day * self.tropic_height / Self::HALF_YEAR
      } else {
		   (h as f32 + self.tropic_height) / 2.0 - (day - Self::HALF_YEAR) * self.tropic_height / Self::HALF_YEAR
      };

      Coord::new(x as usize, y as usize)
   }

   fn shine(&mut self, tick: usize) {
      // Sun position
      let sun_pos = self.sun_position(tick);

      let w = self.world_size.x as isize;
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
   }

   fn diffusion(&mut self, element_index: usize, volatility: f32, rng: &mut ThreadRng) {
      (0..self.num_points_to_diffuse).for_each(|_| {
         // Get a random point
         let bit_num = rng.gen::<usize>() % self.bits_count;
         let origin = self.bits.coord_by_num(bit_num);
         let from_bit = self.bits.bit_mut(&origin);

         // Get an amount of matter with its volatility
         let amount = (from_bit.amount(element_index) as f32 * volatility) as usize;

         if amount > 0 {
            from_bit.add_amount(element_index, -(amount as isize));

            // Get bit at random direction from 0 (up at twelwe) to 7 (at eleven)
            let dir = rng.gen::<usize>() % 8;
            let dest = self.bits.at_direction(&origin, dir.into());
            let to_bit = self.bits.bit_mut(&dest);

            // Move matter
            to_bit.add_amount(element_index, amount as isize);
         }
      })
   }
}
