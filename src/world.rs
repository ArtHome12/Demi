/* ===============================================================================
Simulation of the evolution of the animal world.
The world as a set of dots.
17 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use crate::dot;

pub struct World {
   dots: dot::Dots,
}

impl World {
   pub fn new(width: usize, height: usize, elements_number: usize) -> Self {
      // Model point
      let p = dot::Dot::new(elements_number);

      Self {
         dots: vec![vec![p.clone(); width]; height],
      }
   }
}

impl Default for World {
   fn default() -> Self {
       Self::new(0, 0, 0)
   }
}

