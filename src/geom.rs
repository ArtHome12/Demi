/* ===============================================================================
Simulation of the evolution of the animal world.
Some geometric structures.
25 Mar 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#[derive(Debug, Clone, Copy)]
pub struct Size {
   pub x: usize,
   pub y: usize,
}

pub type Coord = Size;

impl Size {
   pub fn new(x: usize, y: usize) -> Self {
      Self {x, y}
   }
}

pub enum Direction {
   North,
   Northeast,
   East,
   Southeast,
   South,
   Southwest,
   West,
   Northwest,
}

impl From<usize> for Direction {
   fn from(num: usize) -> Direction {
      match num {
         0 => Direction::North,
         1 => Direction::Northeast,
         2 => Direction::East,
         3 => Direction::Southeast,
         4 => Direction::South,
         5 => Direction::Southwest,
         6 => Direction::West,
         7 => Direction::Northwest,
         _ => panic!("From {} for Direction", num),
      }
   }
}
