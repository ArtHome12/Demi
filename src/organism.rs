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

use crate::genes::*;
use crate::reactions::Reactions;
use crate::dot::*;

// pub type Organisms = Vec<Weak<Organism>>;

#[derive(Debug, Clone)]
pub struct Organism {
   pub vitality: usize,
   pub birthday: usize,
   gene_digestion: Digestion,
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


   pub fn digestion(&mut self, ptr: &PtrSheets, serial: usize, reactions: &Reactions) {
      // Check the availability of resources for digestion
      let r = self.gene_digestion.reaction;
      let r = reactions.get(r);
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

   pub fn reaction_index(&self) -> usize {
      self.gene_digestion.reaction
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


#[derive(Clone, Debug)]
// Organisms at one point
pub struct AnimalStack {
   stack: Vec<Option<Organism>>,
}

impl<'s> AnimalStack {

   pub fn new(max_animal_stack: usize) -> Self {
      Self{
         stack: vec![None; max_animal_stack],
      }
   }

   // Returns iterator over alive organism in stack, if such exists
   pub fn get_mut_alive(&'s mut self) -> impl Iterator<Item = &'s mut Organism> {
      self.stack
      .iter_mut()
      .filter_map(|item| item.as_mut().and_then(|o| o.alive().then(|| o)))
   }

   pub fn get_animals(&'s self) -> impl Iterator<Item = &'s Organism> {
      self.stack
      .iter()
      .filter_map(|item| item.as_ref())
   }

   // Return index of first empty or occuped item
   pub fn get_slot_index(&'s self, occuped: bool) -> Option<usize> {
      self.stack
      .iter()
      .position(|opt| opt.is_some() == occuped)
   }

   // pub fn get_slot_mut(&'s mut self, occuped: bool) -> Option<&mut Option<Organism>> {
   //    self.stack
   //    .iter_mut()
   //    .find(|opt| opt.is_some() == occuped)
   // }

   pub fn reproduction(&mut self, now: usize) {
      // Checking for free space at the point
      let free_slot = self.get_slot_index(false);


      if let Some(free_slot) = free_slot {

         // For optimization
         let mut randoms = Randoms::new();

         // Looking for among all living organisms at a point capable of reproducing. The first gets priority
         let opt_new_born = self
         .get_mut_alive()
         .find_map(|animal| animal.reproduction(&mut randoms));

         // Ask about birth
         if let Some(mut new_born) = opt_new_born {
            // Place new organism
            new_born.birthday = now;
            self.stack[free_slot] = Some(new_born);
         };
      }
   }
}


#[derive(Clone, Debug)]
// Keeps live and death organisms in the grid
pub struct AnimalSheet(Vec<AnimalStack>);

impl<'a> AnimalSheet {
   pub fn new(max_serial: usize, max_animal_stack: usize) -> Self {
      let mut matrix = Vec::with_capacity(max_serial);

      // Fill points with stacks for futures animals
      (0..max_serial)
      .for_each(|_| matrix.push(AnimalStack::new(max_animal_stack)));

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


   pub fn digestion(&mut self, elements: &mut Sheets, reactions: &Reactions) {
      let ptr_elements = PtrSheets::new(elements);

      // Each point on the ground
      self.0
      .par_iter_mut()
      .enumerate()
      .for_each(|(serial, animals)| {
         // Each alive organism at the point
         animals
         .get_mut_alive()
         .for_each(|animal| {
            animal.digestion(&ptr_elements, serial, reactions)
         })
      })
   }


   pub fn reproduction(&mut self, now: usize) {
      // Each point on the ground
      self.0
      .par_iter_mut()
      .for_each(|animals| animals.reproduction(now))
   }

   // Try to transfer organism between points
   pub fn transfer(&mut self, origin: usize, dest: usize) {

      // If there is something to transfer and free space at destinaton
      if let Some(source_index) = self.get(origin).get_slot_index(true) {
         if let Some(dest_index) = self.get(dest).get_slot_index(false) {

            // Extract organism from previous place
            let origin_stack = self.get_mut(origin);
            let origin_place = &mut origin_stack.stack[source_index];
            let organism = std::mem::replace(origin_place, None);

            // And put it to the new place
            let dest_stack = self.get_mut(dest);
            dest_stack.stack[dest_index] = organism;
         }
      }
   }

   pub fn implantation(&mut self, luca: &Organism, now: usize) {
      let stack = self.get_mut(0);
      let first_alive = stack.get_mut_alive().next();
      if first_alive.is_none() {

         // Check free place
         if let Some(free_slot) = stack.get_slot_index(false) {
            let mut luca = luca.to_owned();
            luca.birthday = now;
            stack.stack[free_slot] = Some(luca);
         }
      }
   }


   pub fn end_of_turn(&mut self, now: usize) {
      // Each point on the ground
      self.0
      .par_iter_mut()
      .for_each(|stack| {
         // Each alive organism at the point
         stack
         .get_mut_alive()
         .for_each(|animal| {
            // Decrease vitality on methabolism
            animal.vitality = animal.vitality.saturating_sub(max(1, animal.metabolism));

            // In case of death keep the age
            if animal.vitality == 0 {
               animal.birthday = now - animal.birthday + 1;
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

// pub struct PtrAnimalSheet(Vec<Vec<usize>>);
