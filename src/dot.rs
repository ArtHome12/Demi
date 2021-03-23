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
}

// Representation for display
#[derive(Debug, Clone)]
pub struct Dot {
   // Location
   pub x: usize,
   pub y: usize,

   // Color for display
   pub color: Color,
}

// Location separately from Bit, Dot
pub struct Size {
   pub x: usize,
   pub y: usize,
}

pub type Coord = Size;

impl Size {
   pub fn new(x: usize, y: usize) -> Self {
      Self {x, y}
   }
}

#[derive(Clone, Debug)]
pub struct Bits {
   stor: Vec<Vec<Bit>>,
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

      Self {stor}
   }

   pub fn bit(&self, x: usize, y: usize) -> &Bit {
      &self.stor[x][y]
   }

   pub fn set_energy(&mut self, coord: &Coord, energy: usize) {
      let Coord{x, y} = *coord;
      self.stor[x][y].energy = energy;
   }

   pub fn set_amount(&mut self, coord: &Coord, element_index: usize, new_amount: usize) {
      let Coord{x, y} = *coord;
      self.stor[x][y].set_amount(element_index, new_amount);
   }

   pub fn as_slice(&self) -> &[Vec<Bit>] {
      self.stor.as_slice()
   }

   pub fn clone_from_slice(&mut self, slice: &[Vec<Bit>]) {
      self.stor.clone_from_slice(slice);
   }
}