/* ===============================================================================
Simulation of the evolution of the animal world.
Chemical reactions.
21 Apr 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2022 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::sync::Arc;
use std::slice::Iter;
use iced::Color;


// Element-reagent of reaction
#[derive(Debug)]
pub struct Reagent {
   pub index: usize, // index of element
   pub amount: usize, // number of elements to react
}

#[derive(Debug)]
pub struct Reaction {
   pub vitality: usize, // the amount of vitality produced
   pub left: Vec<Reagent>, // initial elements for the reaction
   pub right: Vec<Reagent>, // reaction products

   // For UI
   pub name: String, // Title
   pub color: Color, // color for display
}

#[derive(Debug, Clone)]
pub struct Reactions(Vec<Arc::<Reaction>>);

impl Reactions {
   pub fn find(&self, name: &String) -> Option<&Arc::<Reaction>> {
      self.0.iter()
      .find(|a| {
         name == &a.name
      })
   }

   pub fn iter(&self) -> Iter<'_, Arc::<Reaction>> {
      self.0.iter()
   }
}

impl std::iter::FromIterator<Arc::<Reaction>> for Reactions {
   fn from_iter<I: IntoIterator<Item = Arc::<Reaction>>>(iter: I) -> Self {
      Self {
         0: iter.into_iter()
         .collect(),
      }
   }
}

