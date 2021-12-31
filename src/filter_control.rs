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
   ItemToggled(usize, bool), // index, checked
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
      match message {
         Message::ItemToggled(index, checked) => {
            let mut pr = self.project.borrow_mut();

            // Insert or remove index to the corresponding list
            if checked {
               pr.vis_elem_indexes.push(index);

               // Keep order from first to last to correctly select the color of the first non-empty element
               pr.vis_elem_indexes.sort();
            } else {
               pr.vis_elem_indexes.retain(|value| *value != index);
            }
         }
      }
  }

   pub fn view(&mut self) -> Element<Message> {

      let pr = self.project.borrow();
      
      let check_boxes = pr.elements
         .iter()
         .enumerate()
         .fold(Column::new().spacing(10), |column, (index, item)| {
            column.push(Checkbox::new(
               pr.vis_elem_indexes.contains(&index),
               &item.name,
               move |b| Message::ItemToggled(index, b),
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
         .push(check_boxes);      

      Container::new(content)
         .width(Length::Fill)
         .height(Length::Fill)
         .padding(5)
         .into()
  }
}
