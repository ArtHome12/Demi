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
mod environment;
mod resources;
mod elements_control;

use grid::Grid;
use iced::{Application, Column, Command, Container, Element, Length, Settings,
   Subscription, time, executor, PaneGrid, pane_grid, 
   pane_grid::{Pane, Axis, },
};
use std::{time::{Duration, Instant}};

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

   Dragged(pane_grid::DragEvent),
   Resized(pane_grid::ResizeEvent),
   Filter(elements_control::Message),
}



struct Demi {
   controls: project_controls::Controls,
   panes: pane_grid::State<Content>,
   grid_pane: Pane,
   last_one_second_time: Instant,
}

impl Application for Demi {
   type Message = Message;
   type Executor = executor::Default;
   type Flags = ();

   fn new(_flags: ()) -> (Self, Command<Message>) {
      let project = project::Project::new("./demi.toml");
      let controls = project_controls::Controls::new(resources::Resources::new("./res"));

      // Put the grid into pane and get back ref to it
      let (panes, grid_pane) = pane_grid::State::new(Content::new(PaneContent::Grid(Grid::new(project))));
      (
         Self {
            grid_pane,
            controls,
            panes,
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
         Message::Grid(message) => self.grid_mut().update(message),
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
               self.grid_mut().clock_chime();
            }
         }

         Message::Resized(pane_grid::ResizeEvent { split, ratio }) => {
            self.panes.resize(&split, ratio);
         }
         Message::Dragged(pane_grid::DragEvent::Dropped {
            pane,
            target,
         }) => {
            self.panes.swap(&pane, &target);
         }
         Message::Dragged(_) => {}
         Message::Filter(_message) => {

            // Reflecting the interface change immediately
            // self.controls.update(message.clone());

            // Handle the message
            // self.project_control(message)
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

      // Grid with the world's dots
      // let grid_area = self.grid().view().map(move |message| Message::Grid(message));

      let pane_grid = PaneGrid::new(&mut self.panes, |_pane, content| {
         pane_grid::Content::new(content.view())
      })
      .width(Length::Fill)
      .height(Length::Fill)
      .spacing(10)
      .on_drag(Message::Dragged)
      .on_resize(10, Message::Resized);

      let content = Column::new()
      .push(controls)
      .push(pane_grid);
      // .push(grid_area);

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
         project_controls::Message::ToggleRun => self.grid_mut().world.toggle_pause(),
         project_controls::Message::ToggleIllumination(is_on) => self.grid_mut().set_illumination(is_on),
         project_controls::Message::ToggleFilter(show) => {
            if show {
               self.panes.split(Axis::Vertical, &self.grid_pane, Content::new(PaneContent::Filter(elements_control::Controls::new())));
            } else {
               let filter_pane = self.panes.iter()
               .skip_while(|p| p.0 != &self.grid_pane)
               .last()
               .unwrap()
               .0.clone();
               
               self.panes.close(&filter_pane);
            }
         }
         _ => (),
      }
   }

   fn grid_mut(&mut self) -> &mut Grid {
      match self.panes.get_mut(&self.grid_pane).unwrap().content {
         PaneContent::Grid(ref mut grid) => grid,
         _ => panic!("grid_mut() Pane is not a grid"),
      }
   }
}

enum PaneContent {
   Grid(Grid),
   Filter(elements_control::Controls),
}

struct Content {
   pub content: PaneContent,
}

impl Content {
   fn new(content: PaneContent) -> Self {
       Content {
            content,
       }
   }
   fn view(&mut self,) -> Element<Message> {

      match &mut self.content {
         PaneContent::Grid(grid) => grid.view().map(move |message| Message::Grid(message)),
         PaneContent::Filter(filter) => filter.view().map(move |message| Message::Filter(message)),
      }

   }
}

