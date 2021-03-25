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
mod update_rate;
mod geom;

use grid::Grid;
use iced::{Application, Column, Command, Container, Element, Length, Settings,
   Subscription, time, executor,
};
use std::time::{Duration, Instant};

#[tokio::main]
async fn main() -> iced::Result {
   Demi::run(Settings {
      antialiasing: true,
      ..Settings::default()
   })
}


#[derive(Debug, Clone)]
enum Message {
   ProjectMessage(project_controls::Message),
   Grid(grid::Message),
   Cadence(Instant), // called about 30 times per second for screen refresh
}

struct Demi {
   grid: Grid,
   controls: project_controls::Controls,
   last_one_second_time: Instant,
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
            last_one_second_time: Instant::now(),
         },
         Command::none(),
      )
   }

   fn title(&self) -> String {
      String::from("Demi")
   }

   fn update(&mut self, message: Message) -> Command<Message> {
      match message {
         Message::Grid(message) => self.grid.update(message),
         Message::ProjectMessage(message) => {

            // Reflecting the interface change immediately
            self.controls.update(message.clone());

            // Handle the message
            self.project_control(message)
         }
         Message::Cadence(ts) => {
            // Update rates once a second
            if ts.duration_since(self.last_one_second_time) >= Duration::new(1, 0) {
               self.last_one_second_time = ts;
               self.grid.clock_chime();
            }
         }
      }
      Command::none()
   }

   fn subscription(&self) -> Subscription<Message> {
      time::every(Duration::from_millis(30))
      .map(Message::Cadence)
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
   fn project_control(&mut self, message: project_controls::Message) {
      match message {
         project_controls::Message::ToggleRun => self.grid.world.toggle_pause(),
         project_controls::Message::ToggleIllumination(checked) => self.grid.toggle_illumination(checked),
         _ => (),
      }
   }
}