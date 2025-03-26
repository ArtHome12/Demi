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


pub trait Gene {
   fn mutate(&self, rng: &mut ThreadRng) -> Self;
}

#[derive(Debug, Clone)]
pub struct Digestion {
   pub reaction: usize, // reaction index
   number_of_reactions: usize,
   variability: Uniform<usize>, // number of reactions * 3
}


impl Digestion {
   pub fn new(reaction: usize, number_of_reactions: usize) -> Self {
      Self {
         reaction,
         number_of_reactions,
         variability: Uniform::new(0, number_of_reactions * 3).unwrap(),
      }
   }
}

impl Gene for Digestion {
   fn mutate(&self, rng: &mut ThreadRng) -> Self {

      // Prepare a clone
      let mut res = self.clone();

      // Determine whether there will be a mutation
      let dice = self.variability.sample(rng);
      if dice < self.number_of_reactions && dice != self.reaction{
         res.reaction = dice;
      }

      res
   }
}

#[derive(Debug, Clone)]
pub struct Reproduction {
   pub level: usize, // the level of strength (vitality) required to divide
   variability: Uniform<usize>, // 0..=2
}

impl Reproduction {
   pub fn new(level: usize) -> Self {
      Self {
         level,
         variability: Uniform::try_from(0..=2).unwrap(),
      }
   }
}

impl Gene for Reproduction {
   fn mutate(&self, rng: &mut ThreadRng) -> Self {
      let level = match self.variability.sample(rng) {
         0 => self.level.saturating_sub(1),
         1 => self.level.saturating_add(1),
         _ => self.level,
      };

      Self {
         level,
         variability: self.variability.clone(),
      }
   }
}
