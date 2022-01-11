/* ===============================================================================
Simulation of the evolution of the animal world.
Control for filtering displayed elements.
09 Apr 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2022 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::{rc::Rc, cell::RefCell, };
use iced::{Container, Element, Length, Column, Checkbox, scrollable, Scrollable,
   Align, Text,
};

use crate::world::World;


#[derive(Debug, Clone)]
pub enum Message {
   ItemToggledElement(usize, bool), // index, checked
   ItemToggledAnimal(usize, bool), // index, checked
   ItemToggledDead(bool), // index, checked
}


pub struct Controls {
   scroll: scrollable::State,
   world: Rc<RefCell<World>>,
}

impl Controls {
   pub fn new(world: Rc<RefCell<World>>) -> Self {
      Self{
         scroll: scrollable::State::new(),
         world,
      }
   }

   pub fn update(&mut self, message: Message) {
      // Storage for visible items
      let mut pr = &mut self.world.borrow_mut().project;

      match message {
         Message::ItemToggledElement(index, checked) => pr.vis_elem_indexes[index] = checked,
         Message::ItemToggledAnimal(index, checked) => pr.vis_reac_indexes[index] = checked,
         Message::ItemToggledDead(checked) => pr.vis_dead = checked,
      }
  }

   pub fn view(&mut self) -> Element<Message> {

      let pr = &self.world.borrow().project;
      
      let elements_check_boxes = pr.elements
      .iter()
      .enumerate()
      .fold(Column::new().spacing(10), |column, (index, item)| {
         column.push(Checkbox::new(
            pr.vis_elem_indexes[index],
            &item.name,
            move |b| Message::ItemToggledElement(index, b),
            ).text_size(16)
            .size(16)
         )
      });

      let dead_check_box = Checkbox::new(
         pr.vis_dead,
         "Include dead",
         move |b| Message::ItemToggledDead(b),
         )
      .text_size(16)
      .size(16);

      let animal_check_boxes = pr.reactions
      .iter()
      .enumerate()
      .fold(Column::new().spacing(10), |column, (index, item)| {
         // Reaction name
         column.push(Checkbox::new(
            pr.vis_reac_indexes[index],
            &item.name,
            move |b| Message::ItemToggledAnimal(index, b),
            ).text_size(16)
            .size(16)
         )
      });

      let content = Scrollable::new(&mut self.scroll)
         .width(Length::Fill)
         .align_items(Align::Start)
         .spacing(10)
         // .push(Space::with_height(Length::Units(600)))
         .push(Text::new("Elements to show:").size(16))
         .push(elements_check_boxes)
         .push(Text::new("Animals ro show:").size(16))
         .push(dead_check_box)
         .push(animal_check_boxes);

      Container::new(content)
         .width(Length::Fill)
         .height(Length::Fill)
         .padding(5)
         .into()
  }
}
