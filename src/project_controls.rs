/* ===============================================================================
Simulation of the evolution of the animal world.
Project controls.
13 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2023 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use iced::{Alignment, Command, Element, theme, };
use iced::widget::{checkbox, row, button};

use crate::resources::*;

pub struct Controls {
   run: bool, // run or pause calculation
   autosave: bool, // autosave project on exit
   autorun: bool, // autostart evaluations on load
   pub illuminate: bool, // toggle illuminations only from the sun or total
   pub show_filter: bool, // panel at the left side with elements filter
   res: Resources,
}

impl Controls {

   pub fn new(res: Resources) -> Self {
         Self {
         run: false,
         autosave: false,
         autorun: false,
         illuminate: false,
         show_filter: false,
         res,
      }
   }

   pub fn update(&mut self, message: Message) -> Command<Message> {
      match message {
         Message::ToggleIllumination => self.illuminate = !self.illuminate,
         Message::ToggleFilter => self.show_filter = !self.show_filter,
         Message::ToggleRun => self.run = !self.run,
         Message::ToggleAutosave(checked) => self.autosave = checked,
         Message::ToggleAutorun(checked) => self.autorun = checked,
         _ => ()
      }
      Command::none()
   }

   pub fn view(
      &self,
   ) -> Element<Message> {

      row![
         // Toggle illuminate
         button(
            self.res.image(if self.illuminate {Images::IlluminateOn} else {Images::IlluminateOff})
         )
         .on_press(Message::ToggleIllumination)
         .style(theme::Button::Secondary),

         // Show or hide filter panel
         button(
            self.res.image(if self.show_filter {Images::ShowFilter} else {Images::HideFilter}),
         )
         .on_press(Message::ToggleFilter)
         .style(theme::Button::Secondary),

         // Pause or run
         button(
            self.res.image(if self.run {Images::ModelPlay} else {Images::ModelPause}),
         )
         .on_press(Message::ToggleRun)
         .style(theme::Button::Secondary),

         // Project commands
         button("New")
         .on_press(Message::New)
         .style(theme::Button::Secondary),

         button("Load")
         .on_press(Message::Load)
         .style(theme::Button::Secondary),

         button("Save")
         .on_press(Message::Save)
         .style(theme::Button::Secondary),

         checkbox("Autosave", self.autosave, Message::ToggleAutosave)
         .size(16)
         .spacing(5)
         .text_size(16),
   
         checkbox("Autorun", self.autorun, Message::ToggleAutorun)
         .size(16)
         .spacing(5)
         .text_size(16),
      ]
      .padding(10)
      .spacing(10)
      .align_items(Alignment::Center)
      .into()
   }
}

#[derive(Debug, Clone)]
pub enum Message {
   ToggleIllumination,
   ToggleFilter,
   New,
   Load,
   Save,
   ToggleRun,
   ToggleAutosave(bool),
   ToggleAutorun(bool),
}

