/* ===============================================================================
Simulation of the evolution of the animal world.
Control for filtering displayed elements.
09 Apr 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use iced::{Container, Element, Length, Text,
};


#[derive(Debug, Clone)]
pub enum Message {
}


pub struct Controls {
}

impl Controls {
   pub fn new() -> Self {
      Self{}
   }

   pub fn view(
      &mut self,
   ) -> Element<Message> {

   let content = Text::new("scroll");

   Container::new(content)
      .width(Length::Fill)
      .height(Length::Fill)
      .padding(5)
      .center_y()
      .into()
  }
}
