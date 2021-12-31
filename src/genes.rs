/* ===============================================================================
Simulation of the evolution of the animal world.
Genes.
22 Apr 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::sync::Arc;
use rand::prelude::*;
use rand::distributions::Uniform;

use crate::chemical::*;

// Environment around the organism
/* pub struct State {
   transfer: bool,
   escape: bool,
   digestion: bool,
   attack: bool,
   rush: bool,
   cheese: bool,
} */


pub struct Randoms {
   rng: ThreadRng,
   variability: Uniform<usize>, // 0..=2
}

impl Randoms {
   pub fn new() -> Self {
      Self {
         rng: rand::thread_rng(),
         variability: Uniform::from(0..=2),
      }
   }

   pub fn get_variability(&mut self) -> usize {
      self.variability.sample(&mut self.rng)
   }
}

pub trait Gene {
   fn mutate(&self, randoms: &mut Randoms) -> Self;
}

#[derive(Debug, Clone)]
pub struct Digestion {
   pub reaction: Arc<Reaction>,
}

impl Gene for Digestion {
   fn mutate(&self, _randoms: &mut Randoms) -> Self {
      self.clone()
   }
}

#[derive(Debug, Clone)]
pub struct Reproduction {
   pub level: usize, // the level of strength required to divide
}

impl Gene for Reproduction {
   fn mutate(&self, randoms: &mut Randoms) -> Self {
      let level = match randoms.get_variability() {
         0 => self.level.saturating_sub(1),
         1 => self.level.saturating_add(1),
         _ => self.level,
      };

      Self { level }
   }
}

