/* ===============================================================================
Simulation of the evolution of the animal world.
Grid of world.
14 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use iced::{
   canvas::event::{self, Event},
   canvas::{self, Cache, Canvas, Cursor, Frame, Geometry, Path, Text},
   mouse, Color, Element, HorizontalAlignment, Length, Point, Rectangle,
   Size, Vector, VerticalAlignment,
};
use rustc_hash::FxHashSet;
use std::{ops::RangeInclusive, usize};
use std::time::Duration;

use crate::world::{World,};

pub struct Grid {
   state: State,
   interaction: Interaction,
   life_cache: Cache,
   grid_cache: Cache,
   translation: Vector,
   scaling: f32,
   last_tick_duration: Duration,
   last_queued_ticks: usize,
   world: World,
}

#[derive(Debug, Clone)]
pub enum Message {
   Populate(Cell),
   Unpopulate(Cell),
}

impl Default for Grid {
   fn default() -> Self {
      Self {
         state: State::with_life(vec![]
                  .into_iter()
                  .map(|(i, j)| Cell { i, j })
                  .collect(),
         ),
         interaction: Interaction::None,
         life_cache: Cache::default(),
         grid_cache: Cache::default(),
         translation: Vector::default(),
         scaling: 1.0,
         last_tick_duration: Duration::default(),
         last_queued_ticks: 0,
         // world: World::default(),
         world: World::new(100, 100, 1),
      }
   }
}

impl Grid {
   const MIN_SCALING: f32 = 0.1;
   const MAX_SCALING: f32 = 200.0;


   pub fn update(&mut self, message: Message) {
      match message {
         Message::Populate(cell) => {
            self.state.populate(cell);
            self.life_cache.clear();
         }
         Message::Unpopulate(cell) => {
            self.state.unpopulate(&cell);
            self.life_cache.clear();
         }
      }
   }

   pub fn view<'a>(&'a mut self) -> Element<'a, Message> {
      Canvas::new(self)
         .width(Length::Fill)
         .height(Length::Fill)
         .into()
   }

   fn visible_region(&self, size: Size) -> Region {
      let width = size.width / self.scaling;
      let height = size.height / self.scaling;

      Region {
         // x: -self.translation.x - width / 2.0,
         // y: -self.translation.y - height / 2.0,
         x: -self.translation.x,
         y: -self.translation.y,
         width,
         height,
      }
   }

   fn project(&self, position: Point, size: Size) -> Point {
      let region = self.visible_region(size);

      Point::new(
         position.x / self.scaling + region.x,
         position.y / self.scaling + region.y,
      )
   }
}

impl<'a> canvas::Program<Message> for Grid {
   fn update(
      &mut self,
      event: Event,
      bounds: Rectangle,
      cursor: Cursor,
   ) -> (event::Status, Option<Message>) {
      if let Event::Mouse(mouse::Event::ButtonReleased(_)) = event {
         self.interaction = Interaction::None;
      }

      let cursor_position =
         if let Some(position) = cursor.position_in(&bounds) {
            position
         } else {
            return (event::Status::Ignored, None);
         };

      let cell = Cell::at(self.project(cursor_position, bounds.size()));
      let is_populated = self.state.contains(&cell);

      let (populate, unpopulate) = if is_populated {
         (None, Some(Message::Unpopulate(cell)))
      } else {
         (Some(Message::Populate(cell)), None)
      };

      match event {
         Event::Mouse(mouse_event) => match mouse_event {
            mouse::Event::ButtonPressed(button) => {
                  let message = match button {
                     mouse::Button::Left => {
                        self.interaction = if is_populated {
                              Interaction::Erasing
                        } else {
                              Interaction::Drawing
                        };

                        populate.or(unpopulate)
                     }
                     mouse::Button::Right => {
                        self.interaction = Interaction::Panning {
                              translation: self.translation,
                              start: cursor_position,
                        };

                        None
                     }
                     _ => None,
                  };

                  (event::Status::Captured, message)
            }
            mouse::Event::CursorMoved { .. } => {
                  let message = match self.interaction {
                     Interaction::Drawing => populate,
                     Interaction::Erasing => unpopulate,
                     Interaction::Panning { translation, start } => {
                        self.translation = translation + (cursor_position - start) * (1.0 / self.scaling);
                        self.life_cache.clear();
                        self.grid_cache.clear();

                        None
                     }
                     _ => None,
                  };

                  let event_status = match self.interaction {
                     Interaction::None => event::Status::Ignored,
                     _ => event::Status::Captured,
                  };

                  (event_status, message)
            }
            mouse::Event::WheelScrolled { delta } => match delta {
                  mouse::ScrollDelta::Lines { y, .. } | mouse::ScrollDelta::Pixels { y, .. } => {
                     if y < 0.0 && self.scaling > Self::MIN_SCALING || y > 0.0 && self.scaling < Self::MAX_SCALING {
                        let old_scaling = self.scaling;

                        self.scaling = (self.scaling * (1.0 + y / 30.0))
                        .max(Self::MIN_SCALING)
                        .min(Self::MAX_SCALING);

                        if let Some(cursor_to_center) = cursor.position() {
                              let factor = self.scaling - old_scaling;

                              self.translation = self.translation - Vector::new(
                                 cursor_to_center.x * factor / (old_scaling * old_scaling),
                                 cursor_to_center.y * factor / (old_scaling * old_scaling),
                              );
                        }

                        self.life_cache.clear();
                        self.grid_cache.clear();
                     }

                     (event::Status::Captured, None)
                  }
            },
            _ => (event::Status::Ignored, None),
         },
         _ => (event::Status::Ignored, None),
      }
   }

   fn draw(&self, bounds: Rectangle, cursor: Cursor) -> Vec<Geometry> {
      // let center = Vector::new(bounds.width / 2.0, bounds.height / 2.0);

      let life = self.life_cache.draw(bounds.size(), |frame| {
         let background = Path::rectangle(Point::ORIGIN, frame.size());
         frame.fill(&background, Color::from_rgb8(0x40, 0x44, 0x4B));

         frame.with_save(|frame| {
            // frame.translate(center);
            frame.scale(self.scaling);
            frame.translate(self.translation);
            frame.scale(Cell::SIZE as f32);

            // Region to draw
            let region = self.visible_region(frame.size());

            // Draw each point from the region
            for point in itertools::iproduct!(region.columns(), region.rows()) {
               // Get dot for point (allow display dot outside its real x and y)
               let (x, y) = point;
               let dot = self.world.dot(x, y);

               // Fill cell's area with a primary color
               frame.fill_rectangle(
                  Point::new(x as f32, y as f32),
                  Size::UNIT,
                  dot.color,
               );

               // Draw the text if it fits
               if self.scaling >= 3.0 {
                  frame.with_save(|frame| {
                     frame.translate(Vector::new(0.03, 0.03));
                     frame.fill_text(Text {
                        content: format!("({}, {})", dot.x, dot.y),
                        position: Point::new(x as f32, y as f32),
                        ..Text::default()
                     });
                  });
               }
            }
         });
      });

      let overlay = {
         let mut frame = Frame::new(bounds.size());

         // Get cell at life coordinates (index of cell)
         let hovered_cell =
            // Position relatively bounds
            cursor.position_in(&bounds).map(|position| {
                  // Rescaling to 100%, adding offset from top left point of visible region
                  // and get index of of a cell-sized square
                  Cell::at(self.project(position, frame.size()))
            });

         if let Some(cell) = hovered_cell {
            frame.with_save(|frame| {
                  // frame.translate(center);    // move center of coordinates from default top left to center of frame
                  frame.scale(self.scaling);             // scale to user's choice
                  frame.translate(self.translation);     // consider the offset of the displayed area
                  frame.scale(Cell::SIZE as f32);        // scale so that the cell with its dimensions occupies exactly one unit

                  // Paint over a square of unit size by cell index
                  frame.fill_rectangle(
                     Point::new(cell.j as f32, cell.i as f32),
                     Size::UNIT,
                     Color {
                        a: 0.5,
                        ..Color::BLACK
                     },
                  );
            });
         }

         let text = Text {
            // color: Color::WHITE,
            size: 14.0,
            position: Point::new(frame.width(), frame.height()),
            horizontal_alignment: HorizontalAlignment::Right,
            vertical_alignment: VerticalAlignment::Bottom,
            ..Text::default()
         };

         if let Some(cell) = hovered_cell {
            frame.fill_text(Text {
               // content: format!("({}, {})", cell.j, cell.i),
               content: format!("Translation ({}, {}), cell ({}, {})", self.translation.x, self.translation.y, cell.i, cell.j),
               position: text.position - Vector::new(0.0, 16.0),
                  ..text
            });
         }

         let cell_count = self.state.cell_count();

         frame.fill_text(Text {
            content: format!(
                  "{} cell{} @ {:?} ({})",
                  cell_count,
                  if cell_count == 1 { "" } else { "s" },
                  self.last_tick_duration,
                  self.last_queued_ticks
            ),
            ..text
         });

         frame.into_geometry()
      };

      if self.scaling < 0.2 {
         vec![life, overlay]
      } else {
         let grid = self.grid_cache.draw(bounds.size(), |frame| {
            // frame.translate(center);
            frame.scale(self.scaling);
            frame.translate(self.translation);
            frame.scale(Cell::SIZE as f32);

            let region = self.visible_region(frame.size());
            let rows = region.rows();
            let columns = region.columns();
            let (total_rows, total_columns) =
                  (rows.clone().count(), columns.clone().count());
            let width = 2.0 / Cell::SIZE as f32;
            let color = Color::from_rgb8(70, 74, 83);
            let special_color = Color::from_rgb8(255, 74, 83);

            frame.translate(Vector::new(-width / 2.0, -width / 2.0));

            for row in region.rows() {
               // There must be a special border when crossing the edge of the world
               let color = if row == 0 {special_color} else {color};

               frame.fill_rectangle(
                     Point::new(*columns.start() as f32, row as f32),
                     Size::new(total_columns as f32, width),
                     color,
                  );
            }

            for column in region.columns() {
               // There must be a special border when crossing the edge of the world
               let color = if column == 0 {special_color} else {color};

               frame.fill_rectangle(
                  Point::new(column as f32, *rows.start() as f32),
                  Size::new(width, total_rows as f32),
                  color,
               );
            }
         });

         vec![life, grid, overlay]
      }
   }

   fn mouse_interaction(
      &self,
      bounds: Rectangle,
      cursor: Cursor,
   ) -> mouse::Interaction {
      match self.interaction {
         Interaction::Drawing => mouse::Interaction::Crosshair,
         Interaction::Erasing => mouse::Interaction::Crosshair,
         Interaction::Panning { .. } => mouse::Interaction::Grabbing,
         Interaction::None if cursor.is_over(&bounds) => {
            mouse::Interaction::Crosshair
         }
         _ => mouse::Interaction::default(),
      }
   }
}

#[derive(Default)]
struct State {
   life: Life,
   births: FxHashSet<Cell>,
   is_ticking: bool,
}

impl State {
   pub fn with_life(life: Life) -> Self {
      Self {
         life,
         ..Self::default()
      }
   }

   fn cell_count(&self) -> usize {
      self.life.len() + self.births.len()
   }

   fn contains(&self, cell: &Cell) -> bool {
      self.life.contains(cell) || self.births.contains(cell)
   }

   /* fn cells(&self) -> impl Iterator<Item = &Cell> {
      self.life.iter().chain(self.births.iter())
   } */

   fn populate(&mut self, cell: Cell) {
      if self.is_ticking {
         self.births.insert(cell);
      } else {
         self.life.populate(cell);
      }
   }

   fn unpopulate(&mut self, cell: &Cell) {
      if self.is_ticking {
         let _ = self.births.remove(cell);
      } else {
         self.life.unpopulate(cell);
      }
   }
}

#[derive(Clone, Default)]
pub struct Life {
   cells: FxHashSet<Cell>,
}

impl Life {
   fn len(&self) -> usize {
      self.cells.len()
   }

   fn contains(&self, cell: &Cell) -> bool {
      self.cells.contains(cell)
   }

   fn populate(&mut self, cell: Cell) {
      self.cells.insert(cell);
   }

   fn unpopulate(&mut self, cell: &Cell) {
      let _ = self.cells.remove(cell);
   }

   /* pub fn iter(&self) -> impl Iterator<Item = &Cell> {
      self.cells.iter()
   } */
}

impl std::iter::FromIterator<Cell> for Life {
   fn from_iter<I: IntoIterator<Item = Cell>>(iter: I) -> Self {
      Life {
         cells: iter.into_iter().collect(),
      }
   }
}

impl std::fmt::Debug for Life {
   fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
      f.debug_struct("Life")
         .field("cells", &self.cells.len())
         .finish()
   }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Cell {
   i: isize,
   j: isize,
}

impl Cell {
   const SIZE: usize = 30;

   fn at(position: Point) -> Cell {
      let i = (position.y / Cell::SIZE as f32).ceil() as isize;
      let j = (position.x / Cell::SIZE as f32).ceil() as isize;

      Cell {
         i: i.saturating_sub(1),
         j: j.saturating_sub(1),
      }
   }
}

pub struct Region {
   x: f32,
   y: f32,
   width: f32,
   height: f32,
}

impl Region {
   fn rows(&self) -> RangeInclusive<isize> {
      let first_row = (self.y / Cell::SIZE as f32).floor() as isize;

      let visible_rows =
         (self.height / Cell::SIZE as f32).ceil() as isize;

      first_row..=first_row + visible_rows
   }

   fn columns(&self) -> RangeInclusive<isize> {
      let first_column = (self.x / Cell::SIZE as f32).floor() as isize;

      let visible_columns =
         (self.width / Cell::SIZE as f32).ceil() as isize;

      first_column..=first_column + visible_columns
   }

   /*fn dots(&self) -> impl Iterator<Item = Cell> {
      let rows = self.rows();
      let columns = self.columns();

      itertools::iproduct!(rows, columns).map(|(i, j)| Cell{i, j})
   }*/
   /* fn cull<'a>(
      &self,
      cells: impl Iterator<Item = &'a Cell>,
  ) -> impl Iterator<Item = &'a Cell> {
      let rows = self.rows();
      let columns = self.columns();

      cells.filter(move |cell| {
          rows.contains(&cell.i) && columns.contains(&cell.j)
      })
  } */
}

enum Interaction {
   None,
   Drawing,
   Erasing,
   Panning { translation: Vector, start: Point },
}

