/* ===============================================================================
Simulation of the evolution of the animal world.
Project TOML file read/save/parse.
28 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::{collections::HashMap, };
use std::fs;
use serde_derive::Deserialize;

use crate::geom::*;
use crate::chemical::*;


// Content of a toml project file
#[derive(Deserialize)]
struct Toml {
   pub width: usize,
   pub height_ratio: f32,
   resolution: f32,

   elements: Vec<ElementAttributes>,
   chemical: Vec<ReactionAttributes>,
   colors: HashMap<String, Colors>,
}

type Colors = (u8, u8, u8);


#[derive(Deserialize)]
struct ElementAttributes {
   name: String,
   color: String,
   volatility: f32,
   amount: usize,
}

#[derive(Debug, Deserialize)]
struct ReactionReagent {
   element: String,
   amount: usize,
}

#[derive(Deserialize)]
struct ReactionAttributes {
   name: String,
   energy: usize,
   vitality: usize,
   left: Vec<ReactionReagent>,
   right: Vec<ReactionReagent>,
}

impl Toml {
   pub fn new(filename: &str) -> Self {
      let data = fs::read_to_string(filename).expect("Unable to read project file");
      toml::from_str(&data).unwrap()
   }
}

pub struct Project {
   pub size: Size,
   pub resolution: f32,
   pub elements: Vec<Element>,
   pub reactions: Vec<Reaction>,
   pub vis_elem_indexes: Vec<usize>, // indexes of visible (non-filtered) elements
}

pub struct Element {
   pub name: String,
   pub color: iced::Color,
   pub volatility: f32,
   pub init_amount: usize,
}

impl Project {
   pub fn new(filename: &str) -> Self {

      // Reads reactions reagents
      fn reagents(elements: &Vec<Element>, part: &Vec<ReactionReagent>) -> Vec<Reagent> {
         part.iter().map(|reagent| {

            // Element index from its name
            let index = elements.iter().position(|v| v.name == reagent.element);
            if index.is_none() {
               panic!("Unknown chemical reagent {}", reagent.element);
            };

            Reagent {
               index: index.unwrap(),
               amount: reagent.amount,
            }
         }).collect()
      }

      // Read general data from toml file
      let toml = Toml::new(filename);
      let width = toml.width;
      let size = Size::new(width, (width as f32 * toml.height_ratio) as usize);

      // Map elements from Hash to Vec with colors in internal representation
      let elements: Vec<Element> = toml.elements.iter().map(|val| {
         // Color item from hash by name
         let color = toml.colors.get(&val.color);

         // Color from r, g, b array
         let color = if let Some(color) = color {
            iced::Color::from_rgb8(color.0, color.1, color.2)
         } else {
            iced::Color::BLACK
         };

         Element {
            name: val.name.clone(),
            volatility: val.volatility,
            color,
            init_amount: val.amount,
         }
      }).collect();

      // Read reactions
      let reactions = toml.chemical.iter().map(|val| {
         Reaction {
            energy: val.energy,
            vitality: val.vitality,
            left: reagents(&elements, &val.left),
            right: reagents(&elements, &val.right),
         }
      }).collect();

      // At start all elements should be visible, collect its indexes
      let len = toml.elements.len();
      let vis_elem_indexes = (0..len).collect();

      Self {
         size,
         resolution: toml.resolution,
         elements,
         reactions,
         vis_elem_indexes,
      }
   }
}