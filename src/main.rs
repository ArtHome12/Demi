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
mod filter_control;
mod chemical;
mod genes;
mod organism;

use grid::Grid;
use iced::{Application, Column, Command, Container, Element, Length, Settings,
   Subscription, time, executor, PaneGrid, pane_grid,
   pane_grid::{Pane, Axis, },
};
use std::time::{Duration, Instant};
use std::{rc::Rc, cell::RefCell, };


#[tokio::main]
async fn main() -> iced::Result {
   Demi::run(Settings {
      antialiasing: true,
      ..Settings::default()
   })
}


#[derive(Debug, Clone)]
enum Message {
   ProjectMessage(project_controls::Message), // Messages from project controls at top line
   GridMessage(grid::Message), // Messages from grid
   FilterMessage(filter_control::Message), // Messages from filter pane at right

   Cadence(Instant), // called about 30 times per second for screen refresh

   Resized(pane_grid::ResizeEvent), // Message from panes

   // Commands to execute
   ShowFilter(bool),
   Illuminate(bool),
   ToggleRun,
}


struct Demi {
   controls: project_controls::Controls,
   panes: pane_grid::State<Content>,
   grid_pane: Pane,
   grid_pane_ratio: f32, // Width of grid when filter visible too
   last_one_second_time: Instant,   // for FPS/TPS evaluations
}

impl Application for Demi {
   type Message = Message;
   type Executor = executor::Default;
   type Flags = ();

   fn new(_flags: ()) -> (Self, Command<Message>) {
      let project = project::Project::new("./demi.toml");
      let project = Rc::new(RefCell::new(project));
      let controls = project_controls::Controls::new(resources::Resources::new("./res"));

      // Put the grid into pane and get back ref to it
      let (panes, grid_pane) = pane_grid::State::new(Content::new(PaneContent::Grid(Grid::new(project))));
      (
         Self {
            controls,
            panes,
            grid_pane,
            grid_pane_ratio: 0.8,
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
         Message::ProjectMessage(message) => {

            // Reflecting the interface change immediately
            self.controls.update(message.clone());

            // Translate from project message to own and create new command
            let cmd = match message {
               project_controls::Message::ToggleRun => Some(Message::ToggleRun),
               project_controls::Message::ToggleIllumination(is_on) => Some(Message::Illuminate(is_on)),
               project_controls::Message::ToggleFilter(show) => Some(Message::ShowFilter(show)),
               _ => None,
            };

            // Create a new command if it necessary
            if let Some(cmd) = cmd {
               return Command::perform(async {}, move |_| cmd.clone())
            };
         }

         Message::GridMessage(_message) => (),

         Message::FilterMessage(message) => self.filter_mut().update(message),

         Message::Cadence(ts) => {
            // Update rates once a second
            if ts.duration_since(self.last_one_second_time) >= Duration::new(1, 0) {
               self.last_one_second_time = ts;
               self.grid_mut().clock_chime();
            }
         }

         Message::Resized(pane_grid::ResizeEvent { split, ratio }) => {
            self.grid_pane_ratio = ratio;
            self.panes.resize(&split, ratio);
         }

         Message::ShowFilter(show) => {
            if show {
               // Get ref to the project from the grid
               let project = self.grid_mut().world.project.clone();

               // Construct filter pane
               let content = filter_control::Controls::new(project);
               let content = PaneContent::Filter(content);
               let content = Content::new(content);
               let pair = self.panes.split(Axis::Vertical, &self.grid_pane, content);

               // Restore ratio
               if let Some(pair) = pair {
                  self.panes.resize(&pair.1, self.grid_pane_ratio);
               }
            } else {
               // Find filter pane from panes and close it
               let filter_pane = self.filter_pane();
               self.panes.close(&filter_pane);
            }
         }

         Message::Illuminate(is_on) => self.grid_mut().set_illumination(is_on),

         Message::ToggleRun => self.grid_mut().world.toggle_run(),
      }
      Command::none()
   }

   fn subscription(&self) -> Subscription<Message> {
      time::every(Duration::from_millis(30))
      .map(Message::Cadence)
   }

   fn view(&mut self) -> Element<Message> {

      // Place project controls at top line
      let controls = self.controls.view().map(Message::ProjectMessage);

      // Client area contains grid with the world's dots and sometimes filter for showing elements
      let pane_grid = PaneGrid::new(&mut self.panes, |_pane, content| {
         pane_grid::Content::new(content.view())
      })
      .width(Length::Fill)
      .height(Length::Fill)
      .on_resize(10, Message::Resized);

      let content = Column::new()
      .push(controls)
      .push(pane_grid);

      Container::new(content)
      .width(Length::Fill)
      .height(Length::Fill)
      .style(style::Container)
      .into()
   }
}

impl Demi {
   fn grid_mut(&mut self) -> &mut Grid {
      match self.panes.get_mut(&self.grid_pane).unwrap().content {
         PaneContent::Grid(ref mut grid) => grid,
         _ => panic!("grid_mut() Pane is not a grid"),
      }
   }

   fn filter_pane(&self) -> Pane {
      // Find filter pane from panes - it have to be not a grid pane
      self.panes.iter()
      .find(|p| p.0 != &self.grid_pane)
      .unwrap()
      .0.clone()
   }

   fn filter_mut(&mut self) -> &mut filter_control::Controls {
      let pane = self.filter_pane();
      match self.panes.get_mut(&pane).unwrap().content {
         PaneContent::Filter(ref mut elements) => elements,
         _ => panic!("filter_mut() Pane is not a filter_control::Controls"),
      }
   }
}

enum PaneContent {
   Grid(Grid),
   Filter(filter_control::Controls),
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
         PaneContent::Grid(grid) => grid.view().map(move |message| Message::GridMessage(message)),
         PaneContent::Filter(filter) => filter.view().map(move |message| Message::FilterMessage(message)),
      }
   }
}

