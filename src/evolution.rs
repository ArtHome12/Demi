/* ===============================================================================
Simulation of the evolution of the animal world.
Mechanism of evolution for the world.
03 Mar 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::{ops::{RangeInclusive}, ptr, };
use rand::distributions::{Distribution, Uniform};
use rayon::prelude::*;

use crate::dot::*;
use crate::geom::*;
use crate::environment::*;
use crate::organism::*;

pub struct Evolution {

   // Solar energy, geothermal energy and elements
   pub sheets: Sheets,

   organisms: Organisms,
}

impl Evolution {

   pub fn new(sheets: Sheets) -> Self {

      let luca = Organism {
         vitality: 300,
         birthday: 0,
      };

      Self {
         sheets,
         organisms: vec![luca],
      }
   }

   pub fn make_tick(&mut self, env: &Environment, tick: usize) {
      // Process inanimal
      self.sheets
      .as_parallel_slice_mut()
      .into_par_iter()
      // .as_mut_slice()
      // .into_iter()
      .for_each(|mut sheet| {
         // Irradiate with solar energy or shuffle elements
         if sheet.volatility <= 0.0 {
            Evolution::shine(env, &mut sheet, tick);
         } else {
            Evolution::diffusion(env, &mut sheet);
         }
      });

      // Process animal
      // self.organisms.into_par_iter()

      // Transfer data to mirror if there no delay
      /* if let Ok(ref mut mirror_sheets) = self.mirror.try_lock() {
         // let mut s_iter = self.sheets.data.iter();
         let mut s_iter = self.sheets.iter();
         for m in mirror_sheets.iter_mut() {
            // let mut s = s_iter.next().unwrap().lock().unwrap();
            let mut s = s_iter.next().unwrap();
            m.memcpy_from(&mut s);
         }
      } */
   }

   fn diffusion(env: &Environment, sheet: &mut Sheet) {
      let mut rng = rand::thread_rng();
      let rnd_bit = Uniform::from(0..env.bits_count);
      let rnd_dir = Uniform::from(0..8);

      // Bounds for pointer
      let first = ptr::addr_of_mut!(sheet.matrix[0]);
      let last = unsafe{ first.add(env.bits_count) };

      (0..env.num_points_to_diffuse).for_each(|_| {
         // Get a random point
         let origin_bit = ptr::addr_of_mut!(sheet.matrix[rnd_bit.sample(&mut rng)]);

         let origin_amount = unsafe{ origin_bit.read() };

         // Amount to tranfer based on its volatility
         let share_amount = (origin_amount as f32 * sheet.volatility) as usize;

         // If there is something to transfer
         if share_amount > 0 {
            // Point to transfer amount
            let dir = rnd_dir.sample(&mut rng);
            let dest = env.distance(dir.into());
            let mut dest = origin_bit.wrapping_offset(dest);

            unsafe {
               // Check bounds
               if dest < first {
                  let delta = dest.offset_from(first);   // negative value
                  dest = last.wrapping_offset(delta);
               } else if dest >= last {
                  let delta = dest.offset_from(last);    // positive value
                  dest = first.wrapping_offset(delta);
               }

               // Add to destination and save, how much it turned out
               let old_val = dest.read();
               let new_val = old_val.saturating_add(share_amount);
               std::ptr::write(dest, new_val);
               let actual_share = new_val - old_val;

               // Deduct actual amount from origin
               std::ptr::write(origin_bit, (origin_amount - actual_share) as usize);
            }
         }
      })
   }

   fn shine(env: &Environment, sheet: &mut Sheet, tick: usize) {

      // Inspired by itertools::iproduct!
      type I = RangeInclusive<isize>;
      struct SquareIter {
         a: I,
         a_cur: Option<isize>,
         b: I,
         b_orig: I,
         anchor: Coord,
         size: Size,
         radius: isize,
      }

      impl SquareIter {
         fn new(radius: isize, anchor: Coord, size: Size) -> Self {
            let mut range = -radius..=radius;
            Self {
               a_cur: range.next(),
               a: range.clone(),
               b: range.clone(),
               b_orig: range,
               anchor,
               size,
               radius,
            }
         }
      }

      impl Iterator for SquareIter
      {
         type Item = (usize, usize);

         fn next(&mut self) -> Option<Self::Item> {

            // Evaluate second coordinate
            let j = match self.b.next() {
               None => {
                     // Start again
                     self.b = self.b_orig.clone();
                     match self.b.next() {
                        None => return None,
                        Some(j) => {
                           self.a_cur = self.a.next(); // on next row
                           j
                        }
                     }
               }
               Some(j) => j
            };

            // From index position to horizontal coordinate with correction of the border
            let mut y = j + self.anchor.y as isize;
            if y < 0 {y += self.size.y as isize}
            else if y >= self.size.y as isize {y -= self.size.y as isize}

            match self.a_cur {
               None => None,
               Some(i) => {
                  // Distance from the anchor
                  let d = ((i * i + j * j) as f64).sqrt();

                  // Brightness is inversely proportional to distance
                  let b = if d < self.radius as f64 {
                     ((self.radius as f64 - d) / (self.radius as f64) * 100.0).round()
                  } else {0.0};

                  // From index position to horizontal coordinate with correction of the border
                  let mut x = i + self.anchor.x as isize;
                  if x < 0 {x += self.size.x as isize}
                  else if x >= self.size.x as isize {x -= self.size.x as isize}

                  // Transform x, y to serial number and return brightness
                  let serial = y * self.size.x as isize + x;
                  Some((serial as usize, b as usize))
               }
            }
         }
      }

      let r = env.light_radius as isize;

      // Sun radiation for time of day
      let sun_pos = env.sun_position(tick);
      let sun_area = SquareIter::new(r, sun_pos, env.world_size);
      for bit in sun_area {
         sheet.set(bit.0, bit.1);
      }
   }
}

