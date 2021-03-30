/* ===============================================================================
Simulation of the evolution of the animal world.
Graphic resources.
30 Mar 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use iced::Image;

pub struct Resources {
   illuminate_on: Image,
   illuminate_off: Image,
}

impl Resources {
   pub fn new(dir: &str) -> Self {
      Self {
         illuminate_on: Image::new(String::from(dir) + "/illuminate_on.png"),
         illuminate_off: Image::new(String::from(dir) + "/illuminate_off.png"),
      }
   }
}