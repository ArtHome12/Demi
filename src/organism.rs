/* ===============================================================================
Simulation of the evolution of the animal world.
Organism.
22 Apr 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

// use std::sync::{Arc, Weak};
use crate::geom::*;
use crate::genes::*;
use crate::dot::*;

// pub type Organisms = Vec<Weak<Organism>>;

#[derive(Debug, Clone)]
pub struct Organism {
   vitality: usize,
   birthday: usize,
   gene_digestion: Digestion,
}

impl Organism {
   pub fn new(vitality: usize, birthday: usize, gene_digestion: Digestion,) -> Self {
      Self {
         vitality,
         birthday,
         gene_digestion,
      }
   }

   pub fn alive(&self, ) -> bool {
      self.vitality > 0
   }

   pub fn digestion(&mut self, element_sheets: &mut Sheets, serial: usize) {
      // Check the availability of resources for digestion
      let r = &self.gene_digestion.reaction;

      let avail = r.left.iter().all(|reagent| {
            reagent.amount <= element_sheets.amount(reagent.index, serial)
         });


      // let avail = r.energy <= element_sheets.amount(0, serial)
      //    && r.left.iter().all(|reagent| {
      //       reagent.amount <= element_sheets.amount(reagent.index, serial)
      //    });
      if !avail {
         return
      }

      // Process the reaction

      // Subtract source elements
      r.left.iter()
      .for_each(|reagent| {
         element_sheets.dec_amount(reagent.index, serial, reagent.amount);
      });

      // Add the resulting elements
      r.right.iter()
      .for_each(|reagent| {
         element_sheets.inc_amount(reagent.index, serial, reagent.amount);
      });

      // Increase vitality
      self.vitality += r.vitality;
   }
}

// Organisms at one point
type AnimalStack = Vec<Organism>;

#[derive(Clone, Debug)]
// Keeps live and death organisms in the grid
pub struct AnimalSheet {
   matrix: Vec<AnimalStack>,
}

impl<'a> AnimalSheet {
   pub fn new(size: Size, ) -> Self {

      // Amount of points
      let prod = size.serial();

      let mut matrix = Vec::with_capacity(prod);
      (0..prod).for_each(|_| matrix.push(AnimalStack::new()));

      Self {
         matrix,
      }
   }

   pub fn iter(&self) -> std::slice::Iter<'_, Vec<Organism>> {
      self.matrix.iter()
   }

   pub fn iter_mut(&mut self) -> std::slice::IterMut<'_, Vec<Organism>> {
      self.matrix.iter_mut()
   }

   // Return stack of organisms at point
   pub fn get(&self, index: usize) -> &AnimalStack {
      &self.matrix[index]
   }

   pub fn get_mut(&mut self, index: usize) -> &mut AnimalStack {
      &mut self.matrix[index]
   }
}

// Point of organisms sheet with coordinates
#[derive(Debug, Clone, Copy)]
pub struct AnimalPoint<'a> {
   pub sheet: &'a AnimalSheet,
   pub coord: Coord,
}

impl<'a> AnimalPoint<'a> {
   pub fn get(&self) -> &'a AnimalStack {
      self.sheet.get(self.coord.serial())
   }
}