/* ===============================================================================
Simulation of the evolution of the animal world.
Main window.
25 Jan 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

mod project;
mod style;
mod grid;
mod dot;
mod world;
mod project_toml;

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
   ProjectMessage(project::Message),
   Grid(grid::Message),
}

struct Demi {
   grid: Grid,
   controls: project::Controls,
}

impl Application for Demi {
   type Message = Message;
   type Executor = executor::Default;
   type Flags = ();

   fn new(_flags: ()) -> (Self, Command<Message>) {
      let project = project_toml::TOML::new("./demi.toml");
      (
         Self {
            grid: Grid::new(&project),
            controls: project::Controls::default(),
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
         }

         Message::ProjectMessage(message) => {
            self.controls.update(message);
         }
      }
      Command::none()
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
