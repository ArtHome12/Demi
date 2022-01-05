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

use crate::project;


#[derive(Debug, Clone)]
pub enum Message {
   ItemToggledElement(usize, bool), // index, checked
   ItemToggledAnimal(String, bool), // index, checked
   ItemToggledDead(bool), // index, checked
}


pub struct Controls {
   scroll: scrollable::State,
   project: Rc<RefCell<project::Project>>,
}

impl Controls {
   pub fn new(project: Rc<RefCell<project::Project>>) -> Self {
      Self{
         scroll: scrollable::State::new(),
         project,
      }
   }

   pub fn update(&mut self, message: Message) {
      // Storage for visible items
      let mut pr = self.project.borrow_mut();

      match message {
         Message::ItemToggledElement(index, checked) => {

            // Insert or remove index to the corresponding list
            if checked {
               pr.vis_elem_indexes.push(index);

               // Keep order from first to last to correctly select the color of the first non-empty element
               pr.vis_elem_indexes.sort();
            } else {
               pr.vis_elem_indexes.retain(|value| *value != index);
            }
         }

         Message::ItemToggledAnimal(name, checked) => {
            let r = pr.vis_reac_hash.get_mut(&name).unwrap();
            *r = checked;
         }
         Message::ItemToggledDead(checked) => pr.vis_dead = checked,
      }
  }

   pub fn view(&mut self) -> Element<Message> {

      let pr = self.project.borrow();
      
      let elements_check_boxes = pr.elements
      .iter()
      .enumerate()
      .fold(Column::new().spacing(10), |column, (index, item)| {
         column.push(Checkbox::new(
            pr.vis_elem_indexes.contains(&index),
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
      .fold(Column::new().spacing(10), |column, reaction| {
         // Reaction name
         let name = reaction.name.to_owned();
         let is_checked = *pr.vis_reac_hash.get(&name).unwrap();
         column.push(Checkbox::new(
            is_checked,
            name.to_owned(),
            move |b| Message::ItemToggledAnimal(name.to_owned(), b),
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
