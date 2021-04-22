/* ===============================================================================
Simulation of the evolution of the animal world.
Organism.
22 Apr 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use crate::geom::*;

pub struct Organism {
   pub center: Coord,
   pub vitality: usize,
   pub birthday: usize,
}