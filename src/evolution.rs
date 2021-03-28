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
use rand::{Rng, };
use rayon::prelude::*;

use crate::dot::*;
use crate::environment::*;


pub struct Evolution {

   mirror: Arc<Mutex<Sheets>>, // for display

   // Elements
   sheets: MutSheets,
}

impl Evolution {

   pub fn new(mirror: Arc<Mutex<Sheets>>) -> Self {
      let sheets = MutSheets::new(&mirror.lock().unwrap());
      Self {
         mirror,
         sheets,
      }
   }

   pub fn make_tick(&mut self, env: &Environment, tick: usize) {
      // Process inanimal
      (0..=env.elements_count).into_par_iter().for_each(|i| {
         let mut sheet = self.sheets.data[i].lock().unwrap();
         // Irradiate with solar energy or shuffle elements
         if i == 0 {
            Evolution::shine(env, &mut sheet, tick);
         } else {
            Evolution::diffusion(env, &mut sheet);
         }
      });

      // Transfer data to mirror if there no delay
      if let Ok(ref mut mirror_sheets) = self.mirror.try_lock() {
         let mut s_iter = self.sheets.data.iter();
         for m in mirror_sheets.iter_mut() {
            let mut s = s_iter.next().unwrap().lock().unwrap();
            m.memcpy_from(&mut s);
         }
      }
   }

   fn diffusion(env: &Environment, sheet: &mut Sheet) {
      let mut rng = rand::thread_rng();

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

