/* ===============================================================================
Simulation of the evolution of the animal world.
Piece of territory.
17 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2022 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::ptr;
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

// Storage amount by points of one element
#[derive(Clone, Debug)]
pub struct Sheet {
   pub matrix: Vec<usize>,

   // How much the fraction moves during diffusion, from 0 to 1 for elements and <0 for energy
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

// Fast unsafe access to elements
pub struct PtrSheets {
   ptr: Vec<*const usize>,
}

impl PtrSheets {
   pub fn create(sheets: &Sheets) -> Self {
      // Store raw pointers to elements
      let ptr = sheets.get().iter()
      .map(|sheet| ptr::addr_of!(sheet.matrix[0]))
      .collect();

      Self { ptr }
   }

   pub fn get(&self, element_index: usize, serial: usize) -> usize {
      unsafe{ self.ptr[element_index].add(serial).read() }
   }

   pub fn inc_amount(&self, element_index: usize, serial: usize, delta: usize) {
      unsafe{ 
         let mut dest = self.ptr[element_index].add(serial);
         let new_val = dest.read().saturating_add(delta);
         std::ptr::write(&mut dest, std::ptr::addr_of!(delta));
      }
   }

   pub fn dec_amount(&self, element_index: usize, serial: usize, delta: usize) {
      unsafe{ 
         let mut dest = self.ptr[element_index].add(serial);
         let new_val = dest.read().saturating_sub(delta);
         std::ptr::write(&mut dest, std::ptr::addr_of!(delta));
      }
   }
}