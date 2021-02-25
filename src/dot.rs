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

type Amounts = Vec<f64>;
pub type Bits = Vec<Vec<Bit>>;

// Internal representation for calculations
#[derive(Debug, Clone)]
pub struct Bit {
   // Location
   x: usize,
   y: usize,

   // The amount of minerals, gas, etc
   elements: Amounts,
}

impl Bit {
   pub fn new(x: usize, y: usize, elements_number: usize) -> Self {
      Bit {
         x,
         y,
         elements : vec![0.0; elements_number],
      }
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
