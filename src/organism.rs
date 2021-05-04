/* ===============================================================================
Simulation of the evolution of the animal world.
Organism.
22 Apr 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::sync::{Arc, Weak};
use crate::geom::*;

pub type Organisms = Vec<Weak<Organism>>;

#[derive(Debug, Clone, Copy)]
pub struct Organism {
   pub vitality: usize,
   pub birthday: usize,
}

impl Organism {
   pub fn alive(&self, ) -> bool {
      self.vitality > 0
   }
}

// Keeps live and death organisms in the grid
type AnimalStack = Vec<Arc<Organism>>;

#[derive(Clone, Debug)]
pub struct AnimalSheet {
   pub matrix: Vec<AnimalStack>,
}

impl AnimalSheet {
   pub fn new(size: Size, ) -> Self {

      // Amount of points
      let prod = size.x * size.y;

      let mut matrix = Vec::with_capacity(prod);
      (0..prod).for_each(|_| matrix.push(AnimalStack::new()));

      Self {
         matrix,
      }
   }
}