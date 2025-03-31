/* ===============================================================================
Simulation of the evolution of the animal world.
Main window.
25 Jan 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2023 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

mod project_controls;
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
mod reactions;
mod genes;
mod organism;

use grid::Grid;
use iced::{Element, Subscription, Task, window, window::Id, Theme};
use iced::widget::{column, PaneGrid, pane_grid, responsive, pane_grid::Axis,};
use iced::window::icon;
use std::path::Path;
use std::time::{Duration, Instant};
use std::{rc::Rc, cell::RefCell, };


// #[tokio::main]
pub fn main() -> iced::Result {

   let icon_path = Path::new("./res/icon32.jpg");
   let icon = icon::from_file(icon_path).ok();
   let settings = iced::window::Settings {
      icon,
      ..iced::window::Settings::default()
   };

   iced::application(Demi::title, Demi::update, Demi::view)
      .subscription(Demi::subscription)
      .exit_on_close_request(false)
      .antialiasing(true)
      .theme(|_| Theme::Nightfly)
      .window(settings)
      .run_with(Demi::new)
}

#[derive(Debug, Clone)]
enum Message {
   ProjectMessage(project_controls::Message), // Messages from project controls at top line
   GridMessage(grid::Message), // Messages from grid
   FilterMessage(filter_control::Message), // Messages from filter pane at right

   Refresh(Instant), // called about 30 times per second for screen refresh

   Resized(pane_grid::ResizeEvent), // Message from panes

   // Commands to execute
   ShowFilter(bool),
   Illuminate(bool),
   ToggleRun,
   WorldSave,
   CloseEvent(Id),
}


struct Demi {
   world: Rc<RefCell<world::World>>,
   controls: project_controls::Controls,
   panes: pane_grid::State<PaneState>,
   grid_pane: pane_grid::Pane,
   filter_pane: Option<pane_grid::Pane>,
   grid_pane_ratio: f32, // Width of grid when filter visible too
   last_one_second_time: Instant,   // for FPS/TPS evaluations
}

impl Demi {
   fn new() -> (Self, Task<Message>) {
      // Project contains info for create model
      let project = project::Project::new("./demi.toml");

      // World contains model and manage its evaluation
      let world = world::World::new(project);
      let world = Rc::new(RefCell::new(world));
      let grid_world = Rc::clone(&world);

      let controls = project_controls::Controls::new(resources::Resources::new("./res"));

      // Put the grid into pane and get back ref to it
      let grid = Grid::new(grid_world);
      let (panes, grid_pane) = pane_grid::State::new(PaneState::new(PaneContent::Grid(grid)));
      (
         Self {
            world,
            controls,
            panes,
            grid_pane,
            filter_pane: None,
            grid_pane_ratio: 0.8,
            last_one_second_time: Instant::now(),
         },
         Task::none(),
      )
   }

    fn title(&self) -> String {
      String::from("Demi")
   }

   fn update(&mut self, message: Message) -> Task<Message> {
      match message {
         Message::ProjectMessage(message) => {

            // Reflecting the interface change immediately
            _ = self.controls.update(message.clone());

            // Translate from project message to own and create new command
            let cmd = match message {
               project_controls::Message::ToggleRun => Some(Message::ToggleRun),
               project_controls::Message::ToggleIllumination => Some(Message::Illuminate(self.controls.illuminate)),
               project_controls::Message::ToggleFilter => Some(Message::ShowFilter(self.controls.show_filter)),
               project_controls::Message::Save => Some(Message::WorldSave),
               _ => None,
            };

            // If necessary, try again with a new command.
            if let Some(cmd) = cmd {
               return self.update(cmd)
            }
         }

         Message::GridMessage(message) => self.grid_mut().update(message),
         Message::FilterMessage(message) => {
            self.filter_mut().update(message);

            let grid_cmd = grid::Message::FilterChanged;
            let cmd = Message::GridMessage(grid_cmd);
            return self.update(cmd)
         }

         Message::Refresh(ts) => {
            // Update rates once a second
            let one_second_passed = ts.duration_since(self.last_one_second_time) >= Duration::new(1, 0);
            if one_second_passed {
               self.last_one_second_time = ts;
            }

            let grid_cmd = grid::Message::ClockChime(one_second_passed);
            let cmd = Message::GridMessage(grid_cmd);
            return self.update(cmd)
         }

         Message::Resized(pane_grid::ResizeEvent { split, ratio }) => {
            self.grid_pane_ratio = ratio;
            self.panes.resize(split, ratio);
         }

         Message::ShowFilter(show) => {
            if show {
               // Construct filter pane. Maybe it should be converted to an asynchronous version.
               let w = self.world.clone();
               let content = filter_control::Controls::new(w);
               let content = PaneContent::Filter(content);
               let content = PaneState::new(content);
               let pair = self.panes.split(Axis::Vertical, self.grid_pane, content);

               // Store pane and restore ratio
               self.filter_pane = if let Some(pair) = pair {
                  self.panes.resize(pair.1, self.grid_pane_ratio);
                  Some(pair.0)
               } else {
                  None
               };

            } else {
               // Close filter
               let filter_pane = self.filter_pane.expect("update() Attempt to close non-existent filter_pane");
               self.panes.close(filter_pane);
               self.filter_pane = None;
            }
         }

         Message::Illuminate(is_on) => {
            let grid_cmd = grid::Message::Illumination(is_on);
            let cmd = Message::GridMessage(grid_cmd);
            return self.update(cmd)
         }

         Message::ToggleRun => self.world.borrow().toggle_run(),

         Message::WorldSave => self.world.borrow().save(),

         Message::CloseEvent(_id) => {
            self.world.borrow_mut().shutdown();
            return iced::exit()
        }
      }
      Task::none()
   }

   fn subscription(&self) -> Subscription<Message> {
      let subs = vec![window::frames().map(Message::Refresh),
         window::close_requests().map(Message::CloseEvent),
      ];
      Subscription::batch(subs)
   }

   fn view(&self) -> Element<Message> {

      // Place project controls at top line
      let controls = self.controls.view().map(Message::ProjectMessage);

      // Client area contains grid with the world's dots and sometimes filter for showing elements
      let pane_grid = PaneGrid::new(&self.panes, |_id, pane, _is_maximized| {
         pane_grid::Content::new(responsive(move |size| {
            pane.view(size)
         }))
      })
      .on_resize(10, Message::Resized);

      column![
         controls,
         pane_grid,
      ].into()
   }

   fn grid_mut(&mut self) -> &mut Grid {
      match self.panes.get_mut(self.grid_pane).unwrap().content {
         PaneContent::Grid(ref mut grid) => grid,
         _ => panic!("grid_mut() Pane is not a grid"),
      }
   }

   fn filter_mut(&mut self) -> &mut filter_control::Controls {
      let pane = self.filter_pane.expect("filter_mut() Attempt to get_mut non-existent filter_pane");
      match self.panes.get_mut(pane).unwrap().content {
         PaneContent::Filter(ref mut elements) => elements,
         _ => panic!("filter_mut() Pane is not a filter_control::Controls"),
      }
   }

}

enum PaneContent {
   Grid(Grid),
   Filter(filter_control::Controls),
}

struct PaneState {
   pub content: PaneContent,
}

impl PaneState {
   fn new(content: PaneContent) -> Self {
       PaneState {
            content,
       }
   }
   
   fn view(&self, size: iced::Size) -> Element<Message> {
      match &self.content {
         PaneContent::Grid(grid) => grid.view(size).map(move |message| Message::GridMessage(message)),
         PaneContent::Filter(filter) => filter.view().map(move |message| Message::FilterMessage(message)),
      }
   }
}

