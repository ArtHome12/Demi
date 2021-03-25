/* ===============================================================================
Simulation of the evolution of the animal world.
Piece of territory.
17 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use iced::Color;

use crate::geom::*;

pub type Amounts = Vec<usize>;

// Internal representation for calculations
#[derive(Debug, Clone)]
pub struct Bit {
   // Location
   x: usize,
   y: usize,

   pub energy: usize, // sun or geothermal
   pub elements: Amounts, // the amount of minerals, gas, etc
}

impl Bit {
   pub fn new(x: usize, y: usize, elements: Amounts) -> Self {
      Bit {
         x,
         y,
         energy: 0,
         elements,
      }
   }

   pub fn amount(&self, element_index: usize) -> usize {
      self.elements[element_index]
   }
   pub fn set_amount(&mut self, element_index: usize, amount: usize) {
      self.elements[element_index] = amount;
   }

   pub fn add_amount(&mut self, element_index: usize, amount: isize) {
      let amount_ref = &mut self.elements[element_index];
      *amount_ref = (*amount_ref as isize + amount) as usize;
   }
}

impl From<&Bit> for Coord {
   fn from(bit: &Bit) -> Coord {
      Coord {
         x: bit.x,
         y: bit.y,
      }
   }
}


// Representation for display
#[derive(Debug, Clone, Copy)]
pub struct Dot {
   // Location
   pub x: usize,
   pub y: usize,

   // Color for display
   pub color: Color,
}

#[derive(Clone, Debug)]
pub struct Bits {
   stor: Vec<Vec<Bit>>,

   // Row length and overall number bits for random_bit()
   width: usize,
   height: usize,
   bits_count: usize,
}

impl Bits {
   pub fn new(size: &Size, initial_amounts: &Amounts) -> Self {
      let Size {x: width, y: height} = *size;

      // Create vec for rows
      let mut stor = Vec::with_capacity(height);

      for x in 0..width {
         // Create vec for items
         let row = (0..height).into_iter().map(|y| Bit::new(x, y, initial_amounts.clone())).collect();

         // Put items into result vector
         stor.push(row);
      };

      Self {
         stor,
         width,
         height,
         bits_count: width * height,
      }
   }

   pub fn bit(&self, coord: &Coord) -> &Bit {
      &self.stor[coord.x][coord.y]
   }

   pub fn bit_mut(&mut self, coord: &Coord) -> &mut Bit {
      &mut self.stor[coord.x][coord.y]
   }

   pub fn set_energy(&mut self, coord: &Coord, energy: usize) {
      self.stor[coord.x][coord.y].energy = energy;
   }

   pub fn set_amount(&mut self, coord: &Coord, element_index: usize, new_amount: usize) {
      self.stor[coord.x][coord.y].set_amount(element_index, new_amount);
   }

   pub fn as_slice(&self) -> &[Vec<Bit>] {
      self.stor.as_slice()
   }

   pub fn clone_from_slice(&mut self, slice: &[Vec<Bit>]) {
      self.stor.clone_from_slice(slice);
   }

   pub fn coord_by_num(&self, num: usize) -> Coord {
      Coord {
         x: num % self.width,
         y: num / self.width,
      }
   }

   pub fn at_direction(&mut self, origin: &Coord, direction: Direction) -> Coord {
      let Coord {x, y} = *origin;

      let (x, y) = match direction {
         Direction::North => (
            x,
            if y > 0 {y - 1} else {self.height - 1}
         ),
         Direction::Northeast => (
            if x + 1 < self.width {x + 1} else {0},
            if y > 0 {y - 1} else {self.height - 1}
         ),
         Direction::East => (
            if x + 1 < self.width {x + 1} else {0},
            y
         ),
         Direction::Southeast => (
            if x + 1 < self.width {x + 1} else {0},
            if y + 1 < self.height {y + 1} else {0},
         ),
         Direction::South => (
            x,
            if y + 1 < self.height {y + 1} else {0},
         ),
         Direction::Southwest => (
            if x > 0 {x - 1} else {self.width - 1},
            if y + 1 < self.height {y + 1} else {0},
         ),
         Direction::West => (
            if x > 0 {x - 1} else {self.width - 1},
            y
         ),
         Direction::Northwest => (
            if x > 0 {x - 1} else {self.width - 1},
            if y > 0 {y - 1} else {self.height - 1}
         ),
      };

      Coord{x, y}
   }

}

