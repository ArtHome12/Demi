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
use crate::environment::*;


pub struct Evolution {

   mirror: Arc<Mutex<Sheets>>, // for display

   // Elements
   sheets: Sheets,
}

impl Evolution {

   pub fn new(mirror: Arc<Mutex<Sheets>>) -> Self {
      let raw_sheets = {mirror.lock().unwrap().clone()};
      Self {
         mirror,
         sheets: raw_sheets,
      }
   }

   pub fn make_tick(&mut self, env: &Environment, tick: usize) {
      // Irradiate with solar energy
      Evolution::shine(env, &mut self.sheets[0], tick);

      // Shuffle elements
      let mut rng = rand::thread_rng();
      (0..env.element_count).for_each(|i| {
         Evolution::diffusion(env, &mut self.sheets[i + 1], &mut rng)
      });

      // Transfer data to mirror if there no delay
      if let Ok(ref mut mirror_sheets) = self.mirror.try_lock() {
         mirror_sheets.iter_mut()
         .zip(self.sheets.iter())
         .for_each(|(m, s)| m.memcpy_from(s));
      }
   }

   fn diffusion(env: &Environment, sheet: &mut Sheet, rng: &mut ThreadRng) {
      (0..env.num_points_to_diffuse).for_each(|_| {
         // Get a random point
         let origin_bit = rng.gen::<usize>() % env.bits_count;
         let origin_amount = sheet.get(origin_bit);

         // Amount to tranfer based on its volatility
         let share_amount = (origin_amount as f32 * sheet.volatility) as usize;

         // If there is something to transfer
         if share_amount > 0 {
            // Point to transfer amount
            let dir = rng.gen::<usize>() % 8;
            let dest = env.at_direction(origin_bit, dir.into());

            // Add to destination and save, how much it turned out
            let actual_share = sheet.add(dest, share_amount);

            // Deduct actual amount from origin
            sheet.sub(origin_bit, actual_share);
         }
      })
   }

   fn shine(env: &Environment, sheet: &mut Sheet, tick: usize) {
      // Sun position
      let sun_pos = env.sun_position(tick);

      let w = env.world_size.x as isize;
      let r = env.light_radius as isize;
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
            sheet.set(env.serial(i as usize, j as usize), b as usize);
         }
      }
   }
}

