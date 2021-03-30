/* ===============================================================================
Simulation of the evolution of the animal world.
Graphic resources.
30 Mar 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use iced::{Image, image::Handle, };

pub enum Images {
   IlluminateOn,
   IlluminateOff,
   ShowFilter,
   HideFilter,
}

pub struct Resources {
   illuminate_on: Handle,
   illuminate_off: Handle,
   show_filter: Handle,
   hide_filter: Handle,
}

impl Resources {
   pub fn new(dir: &str) -> Self {
      Self {
         illuminate_on: Handle::from_path(String::from(dir) + "/illuminate_on.png"),
         illuminate_off: Handle::from_path(String::from(dir) + "/illuminate_off.png"),
         show_filter: Handle::from_path(String::from(dir) + "/show_filter.png"),
         hide_filter: Handle::from_path(String::from(dir) + "/hide_filter.png"),
      }
   }

   pub fn image(&self, image: Images) -> Image {
      let handle = match image {
         Images::IlluminateOn => self.illuminate_on.clone(),
         Images::IlluminateOff => self.illuminate_off.clone(),
         Images::ShowFilter => self.show_filter.clone(),
         Images::HideFilter => self.hide_filter.clone(),
      };
      Image::new(handle)
   }
}