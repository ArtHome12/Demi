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

pub type Sheets = Vec<Sheet>;

