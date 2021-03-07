/* ===============================================================================
Simulation of the evolution of the animal world.
Main window.
25 Jan 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

mod project_controls;
mod style;
mod grid;
mod dot;
mod world;
mod project;
mod evolution;

use std::{thread, time};
use grid::Grid;
use iced::executor;
use iced::{Application, Column, Command, Container, Element, Length, Settings, };


pub fn main() -> iced::Result {
   Demi::run(Settings {
      antialiasing: true,
      ..Settings::default()
   })
}


#[derive(Debug, Clone)]
enum Message {
   ProjectMessage(project_controls::Message),
   Grid(grid::Message),
   Executed(bool),
}

struct Demi {
   grid: Grid,
   controls: project_controls::Controls,
}

impl Application for Demi {
   type Message = Message;
   type Executor = executor::Default;
   type Flags = ();

   fn new(_flags: ()) -> (Self, Command<Message>) {
      let project = project::Project::new("./demi.toml");
      (
         Self {
            grid: Grid::new(project),
            controls: project_controls::Controls::default(),
         },
         Command::none(),
      )
   }

   fn title(&self) -> String {
      String::from("Demi")
   }

   fn update(&mut self, message: Message) -> Command<Message> {
      match message {
         Message::Grid(message) => {
            self.grid.update(message);
            Command::none()
         }

         Message::ProjectMessage(message) => {
            
            // Reflecting the interface change immediately
            self.controls.update(message.clone());

            // Create a future for execute task a bit later
            Command::perform(Self::project_control(message), Message::Executed)
         }

         Message::Executed(_) => Command::none()
      }
   }

   fn view(&mut self) -> Element<Message> {

      // Place project controls
      let controls = self.controls.view().map(Message::ProjectMessage);

      // Client area
      let grid_area = self.grid.view().map(move |message| Message::Grid(message));

      let content = Column::new()
      .push(controls)
      .push(grid_area);

      Container::new(content)
      .width(Length::Fill)
      .height(Length::Fill)
      .style(style::Container)
      .into()
   }
}

impl Demi {
   // Project controls async handler
   async fn project_control(message: project_controls::Message) -> bool {
         _ => (),
      true
   }
}