/* ===============================================================================
Simulation of the evolution of the animal world.
Chemical reactions.
21 Apr 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

// Element-reagent of reaction
#[derive(Debug, Clone, Copy)]
pub struct Reagent {
   pub index: usize, // index of element
   pub amount: usize, // number of elements to react
}

#[derive(Debug, Clone)]
pub struct Reaction {
   pub energy: usize, // the amount of energy needed
   pub vitality: usize, // the amount of vitality produced
   pub left: Vec<Reagent>, // initial elements for the reaction
   pub right: Vec<Reagent>, // reaction products
}