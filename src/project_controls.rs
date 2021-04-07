/* ===============================================================================
Simulation of the evolution of the animal world.
Project controls.
13 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
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
   open_button: button::State,
   save_button: button::State,
   save_as_button: button::State,
   restart_button: button::State,
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
         open_button: button::State::default(),
         save_button: button::State::default(),
         save_as_button: button::State::default(),
         restart_button: button::State::default(),
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
               &mut self.new_button,
               Text::new("New"),
         )
         .on_press(Message::New)
         .style(style::Button),
      )

      .push(
         Button::new(
               &mut self.open_button,
               Text::new("Open"),
         )
         .on_press(Message::Open)
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
         Button::new(
               &mut self.save_as_button,
               Text::new("Save as"),
         )
         .on_press(Message::SaveAs)
         .style(style::Button),
      )

      .push(
         Button::new(
               &mut self.restart_button,
               Text::new("Restart"),
         )
         .on_press(Message::Restart)
         .style(style::Button),
      )

      .push(
         Button::new(
               &mut self.pause_button,
               Text::new(if self.run {"Pause"} else {"Resume"}),
         )
         .on_press(Message::ToggleRun)
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
   Open,
   Save,
   SaveAs,
   Restart,
   ToggleRun,
   ToggleAutosave(bool),
   ToggleAutorun(bool),
}

