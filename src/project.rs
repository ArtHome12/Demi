/* ===============================================================================
Simulation of the evolution of the animal world.
Project TOML file read/save/parse.
28 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::collections::HashMap;
use std::fs;
use serde_derive::Deserialize;

use crate::dot::{Amounts};
use crate::geom::*;


// Content of a toml project file
#[derive(Deserialize)]
struct Toml {
   pub width: usize,
   pub height_ratio: f32,
   resolution: f32,

   // geothermal: HashMap<String, Positions>,
   colors: HashMap<String, Colors>,

   elements: Vec<ElementAttributes>,
}

type Colors = (u8, u8, u8);//Vec::<u8>;
// type Positions = (usize, usize);


#[derive(Deserialize)]
struct ElementAttributes {
   name: String,
   color: String,
   volatility: f32,
   amount: usize,
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
}

pub struct Element {
   pub name: String,
   pub color: iced::Color,
   pub volatility: f32,
   pub amount: usize,
}

impl Project {
   pub fn new(filename: &str) -> Self {
      // Read data from toml file
      let toml = Toml::new(filename);
      let width = toml.width;
      let size = Size::new(width, (width as f32 * toml.height_ratio) as usize);

      // Map from Hash to Vec with colors in internal representation
      let elements = toml.elements.iter().map(|val| {
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
            amount: val.amount,
         }
      }).collect();

      Self {
         size,
         resolution: toml.resolution,
         elements,
      }
   }

   pub fn elements_amount(&self) -> Amounts {
      self.elements.iter().map(|f| f.amount).collect()
   }
}