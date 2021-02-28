/* ===============================================================================
Simulation of the evolution of the animal world.
Project TOML file read/save/parse.
28 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::{collections::HashMap, fs};

use serde_derive::Deserialize;

#[derive(Deserialize)]
pub struct TOML {
   pub width: usize,
   pub height_ratio: f32,
   _resolution: f32,

   colors: HashMap<String, Color>,
}

#[derive(Debug, /* Serialize, */ Deserialize)]
struct Color {
   r: u8,
   g: u8,
   b: u8,
}

impl TOML {
   pub fn new(filename: &str) -> Self {
      let data = fs::read_to_string(filename).expect("Unable to read project file");
      // let path = Path::new(filename);
      // let mut file = BufReader::new(File::open(&path)).expect;
      toml::from_str(&data).unwrap()
   }
}