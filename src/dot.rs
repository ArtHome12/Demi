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
   matrix: Vec<usize>,

   // How much the fraction moves during diffusion, from 0 to 1
   pub volatility: f32,
}

pub type Sheets = Vec<Sheet>;

impl Sheet {
   pub fn new(size: Size, initial_amount: usize, volatility: f32) -> Self {

      // Amount of points
      let prod = size.x * size.y;

      let _ = std::vec::from_elem(0, 10);

      Self {
         matrix: vec![initial_amount; prod],
         volatility,
      }
   }

   pub fn get(&self, i: usize) -> usize {
      self.matrix[i]
   }
   pub fn set(&mut self, i: usize, amount: usize) {
      self.matrix[i] = amount;
   }

   // Returns how much is actually added
   pub fn add(&mut self, i: usize, amount: usize) -> usize {
      let new_val = self.matrix[i].saturating_add(amount);
      let res = new_val - self.matrix[i];
      self.matrix[i] = new_val;
      res
   }

   // Returns how much is actually deducted
   pub fn sub(&mut self, i: usize, amount: usize) -> usize {
      let new_val = self.matrix[i].saturating_sub(amount);
      let res = self.matrix[i] - new_val;
      self.matrix[i] = new_val;
      res
   }

   // Fast copy for mirroring
   pub fn memcpy_from(&mut self, other: &Self) {
      // self.matrix.clone_from_slice(other.matrix.as_slice())

      unsafe {
         core::intrinsics::copy_nonoverlapping(other.matrix.as_ptr(), self.matrix.as_mut_ptr(), self.matrix.len());
      }
   }
}