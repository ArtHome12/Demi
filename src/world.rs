/* ===============================================================================
Simulation of the evolution of the animal world.
The world as a set of dots.
17 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use iced::Color;
use crate::dot::{Dot, Bit, Bits,};

pub struct World {
   width: usize,
   height: usize,
   bits: Bits,
}

impl World {
   pub fn new(width: usize, height: usize, elements_number: usize) -> Self {
      // Create vec for rows
      let mut bits = vec![];

      for x in 0..width {
         // Create vec for items
         let row = (0..height).into_iter().map(|y| Bit::new(x, y, elements_number)).collect();

         // Put items into result vector
         bits.push(row);
      };

      Self {
         width,
         height,
         bits,
      }
   }

   /* pub fn dots(&self, region: WorldRegion) -> impl Iterator<Item = Dot> {
      let width = self.width as isize;
      let height = self.height as isize;
   
      itertools::iproduct!(region.h_range, region.v_range).map(move |(mut x, mut y)| {
         // The world must be continuous, the first point goes to the right (or bottom) of the last point again
         while x < 0 {x += width;}
         while x > width {x -= width;}
         while y < 0 {y += height;}
         while y > height {y -= height;}

         Dot{x: x as usize, y: y as usize, color: Color::WHITE,}
      })
   } */

   // Return dot for display position
   // The world must be continuous, the first point goes to the right (or bottom) of the last point again
   pub fn dot(&self, display_x: isize, display_y: isize) -> Dot {
      let width = self.width as isize;
      let height = self.height as isize;
      let mut x = display_x;
      let mut y = display_y;
   
      while x < 0 {x += width;}
      while x > width {x -= width;}
      while y < 0 {y += height;}
      while y > height {y -= height;}

      Dot{x: x as usize, y: y as usize, color: Color::WHITE,}
   }
}

impl Default for World {
   fn default() -> Self {
       Self::new(0, 0, 0)
   }
}
