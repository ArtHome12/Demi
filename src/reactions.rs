/* ===============================================================================
Simulation of the evolution of the animal world.
Chemical reactions.
21 Apr 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2022 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::slice::Iter;
use iced::Color;


// Element-reagent of reaction
#[derive(Debug, Clone, Copy)]
pub struct Reagent {
   pub index: usize, // index of element
   pub amount: usize, // number of elements to react
}

#[derive(Debug, Clone)]
pub struct Reaction {
   pub vitality: usize, // the amount of vitality produced
   pub left: Vec<Reagent>, // initial elements for the reaction
   pub right: Vec<Reagent>, // reaction products
}

#[derive(Debug, Clone)]
pub struct Reactions(Vec<Reaction>);

impl Reactions {
   pub fn get(&self, index: usize) -> &Reaction {
      &self.0[index]
   }

   pub fn len(&self) -> usize {
      self.0.len()
   }
}

impl std::iter::FromIterator<Reaction> for Reactions {
   fn from_iter<I: IntoIterator<Item = Reaction>>(iter: I) -> Self {
      Self {
         0: iter.into_iter()
         .collect(),
      }
   }
}

// For UI
#[derive(Debug, Clone)]
pub struct UIReaction {
   pub name: String, // Title
   pub color: Color, // color for display
}

#[derive(Debug, Clone)]
pub struct UIReactions(Vec<UIReaction>);

impl UIReactions {
   pub fn index(&self, name: &String) -> Option<usize> {
      self.0.iter()
      .position(|r| &r.name == name)
   }

   pub fn iter(&self) -> Iter<'_, UIReaction> {
      self.0.iter()
   }

   pub fn get(&self, index: usize) -> &UIReaction {
      &self.0[index]
   }
}

impl std::iter::FromIterator<UIReaction> for UIReactions {
   fn from_iter<I: IntoIterator<Item = UIReaction>>(iter: I) -> Self {
      Self {
         0: iter.into_iter()
         .collect(),
      }
   }
}

