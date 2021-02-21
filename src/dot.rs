/* ===============================================================================
Simulation of the evolution of the animal world.
Piece of territory.
17 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

type Amounts = Vec<f64>;
pub type Dots = Vec<Vec<Dot>>;

#[derive(Debug, Clone)]
pub struct Dot {
   // The amount of minerals, gas, etc
   elements: Amounts,
}

impl Dot {
   pub fn new(elements_number: usize) -> Self {
      Dot {
         elements : vec![0.0; elements_number],
      }
   }
}