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


#[derive(Default)]
pub struct Controls {
   paused: bool,
   autosave: bool,
   autorun: bool,
   illuminate: bool,

   new_button: button::State,
   open_button: button::State,
   save_button: button::State,
   save_as_button: button::State,
   restart_button: button::State,
   pause_button: button::State,
}

impl Controls {

   pub fn update(&mut self, message: Message) -> Command<Message> {
      match message {
         Message::Pause => self.paused = !self.paused,
         Message::ToggleAutosave(checked) => self.autosave = checked,
         Message::ToggleAutorun(checked) => self.autorun = checked,
         Message::ToggleIllumination(checked) => self.illuminate = checked,
         _ => ()
      }
      Command::none()
   }

   pub fn view<'a>(
      &'a mut self,
   ) -> Element<'a, Message> {
      // Project controls
      let buttons = Row::new()
      .padding(10)
      .spacing(10)
      .align_items(Align::Center)

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
               Text::new(if self.paused {"Resume"} else {"Pause"}),
         )
         .on_press(Message::Pause)
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
      )

      .push(
         Checkbox::new(self.illuminate, "Illumination", Message::ToggleIllumination)
         .size(16)
         .spacing(5)
         .text_size(16),
      );

      buttons.into()

   }
}

#[derive(Debug, Clone)]
pub enum Message {
   New,
   Open,
   Save,
   SaveAs,
   Restart,
   Pause,
   ToggleAutosave(bool),
   ToggleAutorun(bool),
   ToggleIllumination(bool),
}

