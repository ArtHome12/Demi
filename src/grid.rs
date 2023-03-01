/* ===============================================================================
Simulation of the evolution of the animal world.
Grid of world.
14 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2023 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use iced::{
   widget::canvas::event::{self, Event},
   widget::canvas::{self, Cache, Canvas, Cursor, Frame, Geometry, Path, Text, Stroke, },
   mouse, Color, Element, Length, Point, Rectangle,
   Size, Vector, alignment, Theme,
};

use std::{rc::Rc, cell::RefCell};
use std::ops::Range;

use crate::world::{World, };
use crate::update_rate::*;

pub struct Grid {
   // interaction: Interaction,
   life_cache: Cache,
   grid_cache: Cache,
   translation: Vector,
   scale: f32,
   world: Rc<RefCell<World>>,
   world_size: Size,
   fps: RefCell<FPS>,  // screen refresh rate
   tps: TPS, // model time rate, ticks per second
   illumination: bool,
}

#[derive(Debug, Clone)]
pub enum Message {
   Translated(Vector),
   Scaled(f32, Option<Vector>),
}


impl Grid {
   // Default size of one cell
   const CELL_SIZE: f32 = 30.0;

   // Minimum size to draw a text
   const CELL_SIZE_FOR_TEXT: f32 = 120.0;

   // Height for line of text
   const CELL_TEXT_HEIGHT: f32 = 21.0;

   const MIN_SCALING: f32 = 0.9;
   const MAX_SCALING: f32 = 3.0;

   const SCALE_STEP_DOWN: f32 = 0.9;
   const SCALE_STEP_UP: f32 = 1.111111111111;

   const STATUS_BAR_HEIGHT: f32 = 30.0;

   pub fn new(world: Rc<RefCell<World>>) -> Self {
      let crate::geom::Size {x: w, y: h} = world.borrow().size();
      let world_size = Size::<f32>::new(w as f32, h as f32);

      Self {
         // interaction: Interaction::None,
         life_cache: Cache::default(),
         grid_cache: Cache::default(),
         translation: Vector::default(),
         scale: 1.0,
         world,
         world_size,
         fps: RefCell::new(FPS::default()),
         tps: TPS::default(),
         illumination: false,
      }
   }

   pub fn update(&mut self, message: Message) {
      match message {

         Message::Translated(translation) => {
            self.translation = self.adjust_translation(translation);

            self.life_cache.clear();
            self.grid_cache.clear();
         }

         Message::Scaled(scale, translation) => {
            self.scale = scale;

            if let Some(translation) = translation {
               self.translation = self.adjust_translation(translation);
            }
            println!("scroll translation {:?}, scale {}", self.translation, scale);

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
      let width = size.width / self.scale;
      let height = size.height / self.scale;

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
         position.x / self.scale + region.x,
         position.y / self.scale + region.y,
      )
   }

   fn project_to_world(&self, position_in_units: Point) -> (isize, isize) {
      let x = (position_in_units.x / Self::CELL_SIZE).floor() as isize;
      let y = (position_in_units.y / Self::CELL_SIZE).floor() as isize;
      (x, y)
   }

   fn adjust_translation(&self, translation: Vector) -> Vector {
      // To prevent overflow translation in coninious world
      // let s: Vector = (self.world_size * Self::CELL_SIZE).into();
      // translation % s
      translation
   }

   // Update rate counters
   pub fn clock_chime(&mut self) {
      self.fps.borrow_mut().clock_chime();
      self.tps.clock_chime(self.world.borrow().ticks_elapsed())
   }

   pub fn set_illumination(&mut self, checked: bool) {
      self.illumination = checked;
   }

   fn draw_lines(&self, frame: &mut Frame, color: Color, step: Vector, bounds: Rectangle) {
      // Prepare style
      let stroke = Stroke::default()
      .with_width(1.0)
      .with_color(color);

      // Starting position with correction for a negative value
      let mut p = ((self.translation * -1.0 % step) + step) % step * self.scale;
      // let mut p = self.translation;

      // p *= self.scale;

      let step = step * self.scale;

      // Draw vertical lines
      while p.x < bounds.width {
         let from = Point::new(p.x, 0.0);
         let to = Point::new(p.x, bounds.height);
         frame.stroke(&Path::line(from, to), stroke.to_owned());
         p.x += step.x;
      }

      // Draw horizontal lines
      while p.y < bounds.height {
         let from = Point::new(0.0, p.y);
         let to = Point::new(bounds.width, p.y);
         frame.stroke(&Path::line(from, to), stroke.to_owned());
         p.y += step.y;
      }
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
                     let translation = translation - (cursor_position - start) / self.scale;
                     let msg = Message::Translated(translation);

                     (event::Status::Captured, Some(msg))
                  }
                  Interaction::None => (event::Status::Ignored, None),
               }
            }
            mouse::Event::WheelScrolled { delta } => match delta {
               mouse::ScrollDelta::Lines { y, .. } | mouse::ScrollDelta::Pixels { y, .. } => {
                  if y < 0.0 && self.scale > Self::MIN_SCALING || y > 0.0 && self.scale < Self::MAX_SCALING {

                     let step = if y < 0.0 { Self::SCALE_STEP_DOWN } else { Self::SCALE_STEP_UP };
                     let scale = (self.scale * step)
                     .clamp(Self::MIN_SCALING, Self::MAX_SCALING);

                     let translation = 
                        if let Some(cursor) = cursor.position_in(&bounds) {

                           // Window coordinates under mouse pointer(pixels).
                           let cursor = Vector::new(cursor.x, cursor.y);

                           let delta = cursor / self.scale - cursor / scale;
                           let translation = self.translation + delta;

                           let old_x = self.translation.x + cursor.x * self.scale;
                           let new_x = translation.x + cursor.x * scale;
                           println!("old x={}, new x={}", old_x, new_x);

                           Some(translation)
                        } else {
                           None
                        };
                        
                     (event::Status::Captured, Some(Message::Scaled(scale, translation)))
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
            frame.scale(self.scale);
            frame.translate(self.translation);
            frame.scale(Self::CELL_SIZE);

            // Region to draw
            let region = self.visible_region(frame.size());

            // The max number of lines of text to fit
            let pixels = self.scale * Self::CELL_SIZE;
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

      let grid = self.grid_cache.draw(bounds.size(), |frame| {

         // Draw the inner grid if not too small scale
         if self.scale > 0.2 {
            let color = Color::from_rgb8(70, 74, 83);
            let step = Vector::new(Self::CELL_SIZE, Self::CELL_SIZE);
            self.draw_lines(frame, color, step, bounds);
         }

         // Draw outer borders - lines for border around the world
         let color = Color::from_rgb8(255, 74, 83);
         let step = self.world_size * Self::CELL_SIZE;
         self.draw_lines(frame, color, step.into(), bounds);
      });

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
         let (_years, _days) = self.world.borrow().date();
         frame.fill_text(Text{
            position: Point::new(3.0, frame_height - 3.0),
            // content: format!("{}Y:{}D {} FPS {} TPS", years, days, self.fps.borrow().rate, self.tps.rate),
            content: format!("{:?}, scale {}", self.translation, self.scale),
            ..text
         });

         // Get dot below cursor
         if let Some(cursor_position) = cursor.position_in(&bounds) {

            let cursor = Vector::new(cursor_position.x, cursor_position.y);

            let abs_cursor: Vector = self.translation + cursor / self.scale;

            // Cursor at world coordinates
            let x = (abs_cursor.x / Self::CELL_SIZE).floor();
            let y = (abs_cursor.y / Self::CELL_SIZE).floor();

            // let (x, y) = self.project_to_world(self.project(cursor_position, bounds.size()));

            // Tune scale and offset
            frame.with_save(|frame| {
               frame.scale(self.scale);   // scale to user's choice
               frame.translate(self.translation * -1.0); // consider the offset of the displayed area
               frame.scale(Self::CELL_SIZE); // scale so that the cell with its dimensions occupies exactly one unit

               // Paint over a square of unit size
               frame.fill_rectangle(
                  Point::new(x, y),
                  Size::UNIT,
                  Color {
                     a: 0.5,
                     ..Color::WHITE
                     // ..Color::BLACK
                  },
               );
            });

            // Output info at bottom left edge
            let dot = self.world.borrow().dot(x as isize, y as isize);
            let _description = self.world.borrow().description(&dot, 30, ' ');
            frame.fill_text(Text{
               position: Point::new(350.0, frame_height - 3.0),
               content: format!("{}:{} window cursor {}:{} absolute {}:{}", dot.x, dot.y, cursor_position.x, cursor_position.y, abs_cursor.x, abs_cursor.y),
               // position: Point::new(210.0, frame_height - 3.0),
               // content: format!("{}:{} {}", dot.x, dot.y, description),
               ..text
            });
         }

         frame.into_geometry()
      };

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
   fn columns(&self) -> Range<isize> {
      let first_column = self.columns_start() as isize;
      let visible_columns = self.columns_count() as isize;

      first_column..first_column + visible_columns
   }

   fn rows_count(&self) -> usize {
      (self.height / Grid::CELL_SIZE) as usize
   }

   fn rows_start(&self) -> f32 {
      (self.y / Grid::CELL_SIZE).floor()
   }

   fn rows(&self) -> Range<isize> {
      let first_row = self.rows_start() as isize;
      let visible_rows = self.rows_count() as isize;

      first_row..first_row + visible_rows
   }

   fn columns_count(&self) -> usize {
      (self.width / Grid::CELL_SIZE) as usize
   }

   fn columns_start(&self) -> f32 {
      (self.x / Grid::CELL_SIZE).floor()
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

