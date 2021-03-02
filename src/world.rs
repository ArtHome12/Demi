/* ===============================================================================
Simulation of the evolution of the animal world.
The world as a set of dots.
17 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::usize;

use iced::Color;
use crate::dot::{Bit, Bits,};
pub use crate::dot::Dot;
use crate::project;

pub struct World {
   project: project::Project,
   bits: Bits,
}

impl World {
   pub fn new(project: project::Project) -> Self {
      let width = project.width;
      let height= project.height;
      let elements_number = project.elements_number();

      // Create vec for rows
      let mut bits = vec![];

      for x in 0..width {
         // Create vec for items
         let row = (0..height).into_iter().map(|y| Bit::new(x, y, elements_number.clone())).collect();

         // Put items into result vector
         bits.push(row);
      };

      Self {
         project,
         bits,
      }
   }

   // Return dot at display position
   // The world must be continuous, the first point goes to the right (or bottom) of the last point again
   pub fn dot(&self, display_x: isize, display_y: isize) -> Dot {
      let width = self.project.width as isize;
      let height = self.project.height as isize;
      let mut x = display_x;
      let mut y = display_y;

      while x < 0 {x += width;}
      while x >= width {x -= width;}
      while y < 0 {y += height;}
      while y >= height {y -= height;}

      let x = x as usize;
      let y = y as usize;

      // Corresponding bit of the world
      let bit = &self.bits[x][y];
      let color = if bit.amount(0) != 0.0 {self.project.elements[0].color} else {Color::TRANSPARENT};

      Dot{x, y, color,}
   }

   // Return the corresponding bit for the point
   fn bit(&self, dot: &Dot) -> &Bit {
      &self.bits[dot.x][dot.y]
   }

   // Text to describe a point with a size constraint
   pub fn description(&self, dot: &Dot, max_lines: usize, delimiter: char) -> String {
      self.project.elements.iter()
      .take(max_lines - 1) // -1 for energy
      .enumerate()
      .fold(format!("Energy: {}%", 0.0), |acc, (i, element)| {
         let bit = self.bit(dot);
         format!("{}{}{}: {}", acc, delimiter, element.name, bit.elements[i])
      })
      // .map(|(i, element)| {
      //    let bit = self.bit(dot);
      //    format!("{}: {}\n", element.name, bit.elements[i])
      // })
      // .collect()
   }

   // Temporary for testing
   pub fn populate(&mut self, dot: &Dot) {
      let bit = &mut self.bits[dot.x][dot.y];
      bit.set_amount(0, 1.0);
   }
   pub fn unpopulate(&mut self, dot: &Dot) {
      let bit = &mut self.bits[dot.x][dot.y];
      bit.set_amount(0, 0.0);
   }

   pub fn width(&self) -> usize {
      self.project.width
   }

   pub fn height(&self) -> usize {
      self.project.height
   }
}


