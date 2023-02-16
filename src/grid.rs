/* ===============================================================================
Simulation of the evolution of the animal world.
Grid of world.
14 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2022 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use iced::{
   widget::canvas::event::{self, Event},
   widget::canvas::{self, Cache, Canvas, Cursor, Frame, Geometry, Path, Text, Stroke, },
   mouse, Color, Element, Length, Point, Rectangle,
   Size, Vector, alignment, Theme,
};

use std::{ops::RangeInclusive, rc::Rc, cell::RefCell};

use crate::world::{World, };
use crate::update_rate::*;

pub struct Grid {
   // interaction: Interaction,
   life_cache: Cache,
   grid_cache: Cache,
   translation: Vector,
   scaling: f32,
   world: Rc<RefCell<World>>,
   fps: RefCell<FPS>,  // screen refresh rate
   tps: TPS, // model time rate, ticks per second
   illumination: bool,
}

#[derive(Debug, Clone)]
pub enum Message {
   Translated(Vector),
   Scaled(f32, Option<Vector>),
}

// impl Default for Grid {
//    fn default() -> Self {
//        Self::from_preset(Preset::default())
//    }
// }


impl Grid {
   // Default size of one cell
   const CELL_SIZE: f32 = 30.0;

   // Minimum size to draw a text
   const CELL_SIZE_FOR_TEXT: f32 = 120.0;

   // Height for line of text
   const CELL_TEXT_HEIGHT: f32 = 21.0;

   const MIN_SCALING: f32 = 0.1;
   const MAX_SCALING: f32 = 200.0;

   const STATUS_BAR_HEIGHT: f32 = 30.0;

   pub fn new(world: Rc<RefCell<World>>) -> Self {
      Self {
         // interaction: Interaction::None,
         life_cache: Cache::default(),
         grid_cache: Cache::default(),
         translation: Vector::default(),
         scaling: 1.0,
         world,
         fps: RefCell::new(FPS::default()),
         tps: TPS::default(),
         illumination: false,
      }
   }

   pub fn update(&mut self, message: Message) {
      match message {

         Message::Translated(translation) => {
            self.translation = translation;

            self.life_cache.clear();
            self.grid_cache.clear();
         }

         Message::Scaled(scaling, translation) => {
            self.scaling = scaling;

            if let Some(translation) = translation {
               self.translation = translation;
            }

            self.life_cache.clear();
            self.grid_cache.clear();
         }
      }
   }


   pub fn view<'a>(&'a self) -> Element<'a, Message> {
      Canvas::new(self)
         .width(Length::Fill)
         .height(Length::Fill)
         .into()
   }

   fn visible_region(&self, size: Size) -> Region {
      let width = size.width / self.scaling;
      let height = size.height / self.scaling;

      Region {
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

   fn project_to_world(&self, position_in_units: Point) -> (isize, isize) {
      let x = (position_in_units.x / Self::CELL_SIZE).floor() as isize;
      let y = (position_in_units.y / Self::CELL_SIZE).floor() as isize;
      (x, y)
   }

   fn set_translation(&mut self, new_translation: Vector) {
      self.translation = new_translation;

      // World size in pixels
      let crate::geom::Size {x: w, y: h} = self.world.borrow().size();
      let w = w as f32 * Self::CELL_SIZE;
      let h = h as f32 * Self::CELL_SIZE;
      // To prevent overflow translation in coninious world
      if self.translation.x <= 0.0 {self.translation.x += w}
      else if self.translation.x >= w {self.translation.x -= w}

      if self.translation.y <= 0.0 {self.translation.y += h}
      else if self.translation.y >= h {self.translation.y -= h}
   }

   // Update rate counters
   pub fn clock_chime(&mut self) {
      self.fps.borrow_mut().clock_chime();
      self.tps.clock_chime(self.world.borrow().ticks_elapsed())
   }

   pub fn set_illumination(&mut self, checked: bool) {
      self.illumination = checked;
   }
}

impl<'a> canvas::Program<Message> for Grid {

   type State = Interaction;

   fn update(
      &self,
      interaction: &mut Interaction,
      event: Event,
      bounds: Rectangle,
      cursor: Cursor,
   ) -> (event::Status, Option<Message>) {
      if let Event::Mouse(mouse::Event::ButtonReleased(_)) = event {
         *interaction = Interaction::None;
      }

      let cursor_position = if let Some(position) = cursor.position_in(&bounds) {position}
      else {
         return (event::Status::Ignored, None);
      };

      match event {
         Event::Mouse(mouse_event) => match mouse_event {
            mouse::Event::ButtonPressed(button) => {
               match button {
                  mouse::Button::Right => {
                     *interaction = Interaction::Panning {
                           translation: self.translation,
                           start: cursor_position,
                     };
                  }
                  _ => (),
               };

               (event::Status::Captured, None)
            }
            mouse::Event::CursorMoved { .. } => {
                  match *interaction {
                     Interaction::Panning { translation, start } => {
                        Some(Message::Translated(
                           translation
                               + (cursor_position - start)
                                   * (1.0 / self.scaling),
                        ))
                     }
                     _ => None,
                  };

                  let event_status = match *interaction {
                     Interaction::None => event::Status::Ignored,
                     _ => event::Status::Captured,
                  };

                  (event_status, None)
            }
            mouse::Event::WheelScrolled { delta } => match delta {
                  mouse::ScrollDelta::Lines { y, .. } | mouse::ScrollDelta::Pixels { y, .. } => {
                     if y < 0.0 && self.scaling > Self::MIN_SCALING || y > 0.0 && self.scaling < Self::MAX_SCALING {
                        let old_scaling = self.scaling;

                        let scaling = (self.scaling * (1.0 + y / Self::CELL_SIZE))
                           .max(Self::MIN_SCALING)
                           .min(Self::MAX_SCALING);

                           let translation = 
                              if let Some(cursor_to_center) = cursor.position() {
                                 let factor = self.scaling - old_scaling;

                                 Some(
                                    self.translation - Vector::new(
                                       cursor_to_center.x * factor / (old_scaling * old_scaling),
                                       cursor_to_center.y * factor / (old_scaling * old_scaling),
                                    )
                                 )
                              } else {
                                 None
                              };

                        (event::Status::Captured, Some(Message::Scaled(scaling, translation)))


                     } else {
                        (event::Status::Captured, None)
                     }
                  }
            },
            _ => (event::Status::Ignored, None),
         },
         _ => (event::Status::Ignored, None),
      }
   }

   fn draw(
      &self,
      _interaction: &Interaction,
      _theme: &Theme,
      bounds: Rectangle,
      cursor: Cursor,
   ) -> Vec<Geometry> {

      let life = {
         let mut frame = Frame::new(bounds.size());

         let background = Path::rectangle(Point::ORIGIN, frame.size());
         frame.fill(&background, Color::BLACK);

         frame.with_save(|frame| {
            frame.scale(self.scaling);
            frame.translate(self.translation);
            frame.scale(Self::CELL_SIZE);

            // Region to draw
            let region = self.visible_region(frame.size());

            // The max number of lines of text to fit
            let pixels = self.scaling * Self::CELL_SIZE;
            let lines_number = if pixels > Self::CELL_SIZE_FOR_TEXT {
               (pixels / Self::CELL_TEXT_HEIGHT) as usize
            } else {0};

            // Draw each point from the region
            for point in itertools::iproduct!(region.columns(), region.rows()) {
               // Get dot for point (allow display dot outside its real x and y)
               let (x, y) = point;
               let dot = self.world.borrow().dot(x, y);
               let mut color = dot.color;
               if self.illumination {
                  color.a = 1.0;
               }

               // Fill cell's area with a primary color
               frame.fill_rectangle(
                  Point::new(x as f32, y as f32),
                  Size::UNIT,
                  color,
               );

               // Draw the text if it fits
               if lines_number > 0 {
                  frame.with_save(|frame| {
                     frame.translate(Vector::new(0.03, 0.03));
                     frame.fill_text(Text {
                        content: self.world.borrow().description(&dot, lines_number, '\n'),
                        position: Point::new(x as f32, y as f32),
                        ..Text::default()
                     });
                  });
               }
            }
         });

         frame.into_geometry()
      };

      // Update FPS, once upon refresh
      self.fps.borrow_mut().make_tick();

      let overlay = {
         let mut frame = Frame::new(bounds.size());

         // Translucent bar at the bottom of the window
         let frame_width = frame.width();
         let frame_height = frame.height();
         frame.fill_rectangle(
            Point::new(0.0, frame_height - Self::STATUS_BAR_HEIGHT),
            Size::new(frame_width, Self::STATUS_BAR_HEIGHT),
            Color {
               a: 0.9,
               ..Color::BLACK
            }
         );

         // Text object
         let text = Text {
            color: Color::WHITE,
            vertical_alignment: alignment::Vertical::Bottom,
            ..Text::default()
         };

         // Print FPS and model time
         let (years, days) = self.world.borrow().date();
         frame.fill_text(Text{
            position: Point::new(3.0, frame_height - 3.0),
            content: format!("{}Y:{}D {} FPS {} TPS", years, days, self.fps.borrow().rate, self.tps.rate),
            ..text
         });

         // Get dot below cursor
         if let Some(cursor_position) = cursor.position_in(&bounds) {

            // Cursor at world coordinates
            let (x, y) = self.project_to_world(self.project(cursor_position, bounds.size()));

            // Tune scale and offset
            frame.with_save(|frame| {
               frame.scale(self.scaling);             // scale to user's choice
               frame.translate(self.translation);     // consider the offset of the displayed area
               frame.scale(Self::CELL_SIZE);        // scale so that the cell with its dimensions occupies exactly one unit

               // Paint over a square of unit size
               frame.fill_rectangle(
                  Point::new(x as f32, y as f32),
                  Size::UNIT,
                  Color {
                     a: 0.5,
                     ..Color::BLACK
                  },
               );
            });

            // Output info at bottom left edge
            let dot = self.world.borrow().dot(x as isize, y as isize);
            let description = self.world.borrow().description(&dot, 30, ' ');
            frame.fill_text(Text{
               position: Point::new(210.0, frame_height - 3.0),
               content: format!("{}:{} {}", dot.x, dot.y, description),
               ..text
            });
         }

         frame.into_geometry()
      };

      let grid = self.grid_cache.draw(bounds.size(), |frame| {
         frame.scale(self.scaling);
         frame.translate(self.translation);
         frame.scale(Self::CELL_SIZE);

         let region = self.visible_region(frame.size());
         let rows = region.rows();
         let columns = region.columns();
         let (total_rows, total_columns) = (rows.clone().count(), columns.clone().count());
         let (rows_start, columns_start) = (*rows.start() as f32, *columns.start() as f32);
         let crate::geom::Size {x: world_width, y: world_height} = self.world.borrow().size();

         // Amount of lines for border around the world
         let outer_rows = total_rows / world_height;
         let outer_columns = total_columns / world_width;

         // Color for world's borders
         let special_color = Color::from_rgb8(255, 74, 83);

         // No grid at small scale, only outer border
         if self.scaling < 0.2 {
            // Prepare style
            let stroke = Stroke::default()
            .with_width(1.0)
            .with_color(special_color);

           // Draw horizontal lines
           for row in 0..=outer_rows {
               let from = Point::new(columns_start, (row * world_height) as f32);
               let to = Point::new(total_columns as f32, (row * world_height) as f32);
               frame.stroke(&Path::line(from, to), stroke.to_owned());
            }

            // Draw vertical lines
            for column in 0..=outer_columns {
               let from = Point::new((column * world_width) as f32, rows_start);
               let to = Point::new((column * world_width) as f32, total_rows as f32);
               frame.stroke(&Path::line(from, to), stroke.to_owned());
            }
         } else {
            // Draw the inner grid
            let width = 2.0 / Self::CELL_SIZE;
            let color = Color::from_rgb8(70, 74, 83);

            frame.translate(Vector::new(-width / 2.0, -width / 2.0));

           // Draw horizontal lines
           for row in rows {
               // There must be a special border when crossing the edge of the world, skipping for optimization
               if row != 0 {
                  frame.fill_rectangle(
                     Point::new(columns_start, row as f32),
                     Size::new(total_columns as f32, width),
                     color,
                  );
               }
            }

            // Draw vertical lines
            for column in columns {
               // There must be a special border when crossing the edge of the world, skipping for optimization
               if column != 0 {
                  frame.fill_rectangle(
                     Point::new(column as f32, rows_start),
                     Size::new(width, total_rows as f32),
                     color,
                  );
               }
            }

            // Draw outer borders - horizontal lines
            for row in 0..=outer_rows {
               frame.fill_rectangle(
                  Point::new(columns_start, (row * world_height) as f32),
                  Size::new(total_columns as f32, width),
                  special_color,
               );
            }

            // Draw outer borders - vertical lines
            for column in 0..=outer_columns {
               frame.fill_rectangle(
                  Point::new((column * world_width) as f32, rows_start),
                  Size::new(width, total_rows as f32),
                  special_color,
               );
            }
         }
      });

      vec![life, grid, overlay]
   }

   fn mouse_interaction(
      &self,
      interaction: &Interaction,
      _bounds: Rectangle,
      _cursor: Cursor,
   ) -> mouse::Interaction {
      match interaction {
         Interaction::Panning { .. } => mouse::Interaction::Grabbing,
         _ => mouse::Interaction::default(),
      }
   }
}


#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Cell {
   i: isize,
   j: isize,
}

pub struct Region {
   x: f32,
   y: f32,
   width: f32,
   height: f32,
}

impl Region {
   fn rows(&self) -> RangeInclusive<isize> {
      let first_row = (self.y / Grid::CELL_SIZE).floor() as isize;

      let visible_rows =
         (self.height / Grid::CELL_SIZE).ceil() as isize;

      first_row..=first_row + visible_rows
   }

   fn columns(&self) -> RangeInclusive<isize> {
      let first_column = (self.x / Grid::CELL_SIZE).floor() as isize;

      let visible_columns =
         (self.width / Grid::CELL_SIZE).ceil() as isize;

      first_column..=first_column + visible_columns
   }
}

pub enum Interaction {
   None,
   Panning { translation: Vector, start: Point },
}

impl Default for Interaction {
   fn default() -> Self {
       Self::None
   }
}

