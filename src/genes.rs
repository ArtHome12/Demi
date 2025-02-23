/* ===============================================================================
Simulation of the evolution of the animal world.
Genes.
22 Apr 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2022 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use rand::prelude::*;
use rand::distr::Uniform;


pub struct Randoms {
   rng: ThreadRng,
   variability: Uniform<usize>, // 0..=2
}

impl Randoms {
   pub fn new() -> Self {
      Self {
         rng: rand::rng(),
         variability: Uniform::try_from(0..=2).unwrap(),
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
   pub reaction: usize, // reaction index
}

impl Gene for Digestion {
   fn mutate(&self, _randoms: &mut Randoms) -> Self {
      self.clone()
   }
}

#[derive(Debug, Clone)]
pub struct Reproduction {
   pub level: usize, // the level of strength (vitality) required to divide
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
