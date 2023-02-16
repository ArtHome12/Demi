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
#[derive(Debug)]
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
   pub fn get(&self) -> &Vec<Sheet> {
      &self.0
   }

   pub fn get_mut(&mut self) -> &mut Vec<Sheet> {
      &mut self.0
   }
}

impl std::iter::FromIterator<Sheet> for Sheets {
   fn from_iter<I: IntoIterator<Item = Sheet>>(iter: I) -> Self {
      Self(iter.into_iter().collect())
   }
}

// Fast unsafe access to elements for mirroring and rayon
pub struct PtrSheets(Vec<usize>);

impl PtrSheets {
   pub fn new(sheets: &Sheets) -> Self {
      // Store raw pointers to elements
      let ptr = sheets.get().iter()
      .map(|sheet| ptr::addr_of!(sheet.matrix[0]) as usize)
      .collect();

      Self(ptr)
   }

   pub fn get(&self, element_index: usize, serial: usize) -> usize {
      unsafe{ (self.0[element_index] as *const usize).add(serial).read() }
   }

   pub fn inc_amount(&self, element_index: usize, serial: usize, delta: usize) {
      unsafe{ 
         let dest = (self.0[element_index] as *mut usize).add(serial);
         let new_val = dest.read().saturating_add(delta);
         std::ptr::write(dest, new_val);
      }
   }

   pub fn dec_amount(&self, element_index: usize, serial: usize, delta: usize) {
      unsafe{
         let dest = (self.0[element_index] as *mut usize).add(serial);
         let new_val = dest.read().saturating_sub(delta);
         std::ptr::write(dest, new_val);
      }
   }
}