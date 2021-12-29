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
pub struct Sheet {
   pub matrix: Vec<usize>,

   // How much the fraction moves during diffusion, from 0 to 1
   pub volatility: f32,
}

impl Sheet {
   pub fn new(size: Size, initial_amount: usize, volatility: f32) -> Self {

      // Amount of points
      let prod = size.x * size.y;

      Self {
         matrix: vec![initial_amount; prod],
         volatility,
      }
   }

   pub fn set(&mut self, i: usize, amount: usize) {
      self.matrix[i] = amount;
   }
}

pub struct Sheets(Vec<Sheet>);

impl Sheets {
   pub fn amount(&self, sheet_index: usize, point_serial: usize) -> usize {
      self.0[sheet_index].matrix[point_serial]
   }

   pub fn dec_amount(&mut self, sheet_index: usize, point_serial: usize, delta: usize) -> usize {
      let data = &mut self.0[sheet_index].matrix[point_serial];
      *data = *data - delta;
      *data
   }

   pub fn inc_amount(&mut self, sheet_index: usize, point_serial: usize, delta: usize) -> usize {
      let data = &mut self.0[sheet_index].matrix[point_serial];
      *data = *data + delta;
      *data
   }

   pub fn get(&self) -> &Vec<Sheet> {
      &self.0
   }

   pub fn get_mut(&mut self) -> &mut Vec<Sheet> {
      &mut self.0
   }
}

impl std::iter::FromIterator<Sheet> for Sheets {
   fn from_iter<I: IntoIterator<Item = Sheet>>(iter: I) -> Self {
      Self {
         0: iter.into_iter().collect(),
      }
   }
}
