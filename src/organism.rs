/* ===============================================================================
Simulation of the evolution of the animal world.
Organism.
22 Apr 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2022 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::cmp::max;
use std::fmt;
use rayon::prelude::*;

use crate::geom::*;
use crate::genes::*;
use crate::dot::*;

// pub type Organisms = Vec<Weak<Organism>>;

#[derive(Debug, Clone)]
pub struct Organism {
   pub vitality: usize,
   pub birthday: usize,
   pub gene_digestion: Digestion,
   metabolism: usize,
   gene_reproduction: Reproduction,
}

impl Organism {
   pub fn new(vitality: usize, birthday: usize, gene_digestion: Digestion, gene_reproduction: Reproduction,) -> Self {
      Self {
         vitality,
         birthday,
         metabolism: 0,
         gene_digestion,
         gene_reproduction,
      }
   }


   pub fn alive(&self, ) -> bool {
      self.vitality > 0
   }


   pub fn digestion(&mut self, ptr: &PtrSheets, serial: usize) {
      // Check the availability of resources for digestion
      let r = &self.gene_digestion.reaction;
      let avail = r.left.iter().all(|reagent| {
            reagent.amount <= ptr.get(reagent.index, serial)
         });
      if !avail {
         return
      }

      // Process the reaction

      // Subtract source elements
      r.left.iter()
      .for_each(|reagent| {
         ptr.dec_amount(reagent.index, serial, reagent.amount);
      });

      // Add the resulting elements
      r.right.iter()
      .for_each(|reagent| {
         ptr.inc_amount(reagent.index, serial, reagent.amount);
      });

      // Increase vitality
      self.vitality = self.vitality.saturating_add(r.vitality);
   }


   pub fn reproduction(&mut self, randoms: &mut Randoms) -> Option<Organism> {
      // Check avaliability
      let level = self.gene_reproduction.level;
      if self.vitality > level {
         // Reducing own reserves of vitality
         self.vitality -= level;

         // Bring into the world a new organism
         let res = Self {
            vitality: level / 2,
            birthday: 0,
            metabolism: 0,
            gene_digestion: self.gene_digestion.mutate(randoms),
            gene_reproduction: self.gene_reproduction.mutate(randoms),
         };
         Some(res)

      } else {
         None
      }
   }


   pub fn color(&self) -> iced::Color {
      self.gene_digestion.reaction.color
   }
}


impl fmt::Display for Organism {
   // This trait requires `fmt` with this exact signature.
   fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
       // Write strictly the first element into the supplied output
       // stream: `f`. Returns `fmt::Result` which indicates whether the
       // operation succeeded or failed. Note that `write!` uses syntax which
       // is very similar to `println!`.
       write!(f, "{}❤ {}ߐ", self.vitality, self.gene_reproduction.level)
   }
}


// Organisms at one point
type AnimalStack = Vec<Organism>;

#[derive(Clone, Debug)]
// Keeps live and death organisms in the grid
pub struct AnimalSheet(Vec<AnimalStack>);

impl<'a> AnimalSheet {
   pub fn new(size: Size, ) -> Self {

      // Amount of points
      let prod = size.serial();

      let mut matrix = Vec::with_capacity(prod);
      (0..prod).for_each(|_| matrix.push(AnimalStack::new()));

      Self {
         0: matrix,
      }
   }


   // Return stack of organisms at point
   pub fn get(&self, index: usize) -> &AnimalStack {
      &self.0[index]
   }


   pub fn get_mut(&mut self, index: usize) -> &mut AnimalStack {
      &mut self.0[index]
   }


   pub fn digestion(&mut self, elements: &mut Sheets) {
      let ptr_elements = PtrSheets::create(elements);

      // Each point on the ground
      self.0
      .par_iter_mut()
      .enumerate()
      .for_each(|(serial, animals)| {
         // Each organism at the point
         animals
         .iter_mut()
         .filter(|animal| animal.alive())
         .for_each(|animal| {
            animal.digestion(&ptr_elements, serial)
         })
      })
   }


   pub fn reproduction(&mut self, now: usize) {
      // Each point on the ground
      self.0
      .par_iter_mut()
      .for_each(|animals| {
         // For optimization
         let mut randoms = Randoms::new();
         let mut newborns = AnimalStack::new();

         // Each organism at the point
         animals
         .iter_mut()
         .filter(|animal| animal.alive())
         .for_each(|animal| {
            // Ask about birth
            if let Some(mut new_born) = animal.reproduction(&mut randoms) {
               // Place new organism
               new_born.birthday = now;
               newborns.push(new_born)
            }
         });

         animals.append(&mut newborns);
      })
   }


   pub fn end_of_turn(&mut self, now: usize) {
      // Each point on the ground
      self.0
      .par_iter_mut()
      .for_each(|animals| {
         // Each organism at the point
         animals
         .iter_mut()
         .for_each(|animal| {
            if animal.alive() {
               // Decrease vitality on methabolism
               animal.vitality = animal.vitality.saturating_sub(max(1, animal.metabolism));

               // In case of death keep the age
               if animal.vitality == 0 {
                  animal.birthday = now - animal.birthday + 1;
               }
            }
         })
      })
   }
}



// Point of organisms sheet with coordinates
// #[derive(Debug, Clone, Copy)]
// pub struct AnimalPoint<'a> {
//    pub sheet: &'a AnimalSheet,
//    pub coord: Coord,
// }

// impl<'a> AnimalPoint<'a> {
//    pub fn get(&self) -> &'a AnimalStack {
//       self.sheet.get(self.coord.serial())
//    }
// }
