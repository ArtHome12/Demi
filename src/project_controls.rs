/* ===============================================================================
Simulation of the evolution of the animal world.
Project controls.
13 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2022 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use iced::button::{self, Button};
use iced::{Checkbox, Align, Command, Element, Row, Text,};

use crate::style;
use crate::resources::*;

pub struct Controls {
   run: bool,
   autosave: bool,
   autorun: bool,
   illuminate: bool,
   pub show_filter: bool, // panel at the left side with elements filter

   illuminate_button: button::State,
   filter_button: button::State,

   new_button: button::State,
   load_button: button::State,
   save_button: button::State,
   pause_button: button::State,

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
         illuminate_button: button::State::default(),
         filter_button: button::State::default(),
         new_button: button::State::default(),
         load_button: button::State::default(),
         save_button: button::State::default(),
         pause_button: button::State::default(),
         res,
      }
   }

   pub fn update(&mut self, message: Message) -> Command<Message> {
      match message {
         Message::ToggleIllumination(checked) => self.illuminate = checked,
         Message::ToggleFilter(checked) => self.show_filter = checked,
         Message::ToggleRun => self.run = !self.run,
         Message::ToggleAutosave(checked) => self.autosave = checked,
         Message::ToggleAutorun(checked) => self.autorun = checked,
         _ => ()
      }
      Command::none()
   }

   pub fn view(
      & mut self,
   ) -> Element<Message> {
      // Project controls
      let buttons = Row::new()
      .padding(10)
      .spacing(10)
      .align_items(Align::Center)

      .push(
         Button::new(
               &mut self.illuminate_button,
               self.res.image(if self.illuminate {Images::IlluminateOn} else {Images::IlluminateOff}),
         )
         .on_press(Message::ToggleIllumination(!self.illuminate))
         .style(style::Button),
      )

      .push(
         Button::new(
               &mut self.filter_button,
               self.res.image(if self.show_filter {Images::ShowFilter} else {Images::HideFilter}),
         )
         .on_press(Message::ToggleFilter(!self.show_filter))
         .style(style::Button),
      )

      .push(
         Button::new(
               &mut self.pause_button,
               self.res.image(if self.run {Images::ModelPlay} else {Images::ModelPause}),
         )
         .on_press(Message::ToggleRun)
         .style(style::Button),
      )

      .push(
         Button::new(
               &mut self.new_button,
               Text::new("New"),
         )
         .on_press(Message::New)
         .style(style::Button),
      )

      .push(
         Button::new(
               &mut self.load_button,
               Text::new("Load"),
         )
         .on_press(Message::Load)
         .style(style::Button),
      )

      .push(
         Button::new(
               &mut self.save_button,
               Text::new("Save"),
         )
         .on_press(Message::Save)
         .style(style::Button),
      )

      .push(
         Checkbox::new(self.autosave, "Autosave", Message::ToggleAutosave)
         .size(16)
         .spacing(5)
         .text_size(16),
      )

      .push(
         Checkbox::new(self.autorun, "Autorun", Message::ToggleAutorun)
         .size(16)
         .spacing(5)
         .text_size(16),
      );

      buttons.into()
   }
}

#[derive(Debug, Clone)]
pub enum Message {
   ToggleIllumination(bool),
   ToggleFilter(bool),
   New,
   Load,
   Save,
   ToggleRun,
   ToggleAutosave(bool),
   ToggleAutorun(bool),
}

