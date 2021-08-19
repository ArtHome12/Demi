/* ===============================================================================
Simulation of the evolution of the animal world.
Genes.
22 Apr 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::sync::Arc;
use crate::chemical::*;

// Environment around the organism
/* pub struct State {
   transfer: bool,
   escape: bool,
   digestion: bool,
   attack: bool,
   rush: bool,
   cheese: bool,
} */

pub trait Gene {
   fn behavior(&self);
}

#[derive(Debug, Clone)]
pub struct Digestion {
   pub reaction: Arc<Reaction>,
}

impl Gene for Digestion {
   fn behavior(&self) {

   }
}