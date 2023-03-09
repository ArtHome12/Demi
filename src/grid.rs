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
   mouse, Color, Element, Length, Point, Rectangle, Vector, Size, alignment, Theme,
};
use std::{rc::Rc, cell::RefCell};
use euclid::*;

use crate::world::{World, };
use crate::update_rate::*;

pub struct Grid {
   life_cache: Cache,
   grid_cache: Cache,
   translation: PointW,

   scale: ScaleWS,
   min_scale: ScaleWS,
   max_scale: ScaleWS,
   scale_up: f32,
   scale_down: f32,

   world: Rc<RefCell<World>>,
   nonscaled_size: SizeW, // for performance, world size * CELL_SIZE
   fps: RefCell<FPS>,  // screen refresh rate
   tps: TPS, // model time rate, ticks per second
   illumination: bool,
   last_tick: usize, // for reset life_cache
}

#[derive(Debug, Clone)]
pub enum Message {
   Translated(PointW),
   Scaled(ScaleWS, Option<PointW>),
   ClockChime, // Update rate counters
   Illumination(bool),
   FilterChanged,
}

// Adress spaces for Euclid crate
pub struct ScreenSpace;   // Screen space
pub struct WorldSpace;   // World space

// Shortcuts
type SizeS = Size2D<f32, ScreenSpace>;
type SizeW = Size2D<f32, WorldSpace>;
type PointS = Point2D<f32, ScreenSpace>;
type PointW = Point2D<f32, WorldSpace>;
type ScaleWS = Scale<f32, WorldSpace, ScreenSpace>;

impl Grid {
   // Default size of one cell
   const CELL_SIZE: f32 = 30.0;

   // Minimum size to draw a text
   const CELL_SIZE_FOR_TEXT: f32 = 120.0;

   // Height for line of text
   const CELL_TEXT_HEIGHT: f32 = 21.0;

   // If less, cell borders are not drawn
   const MIN_VISIBLE_CELL_BORDER: f32 = 5.0;

   // Screen size defines scaling range
   const MIN_SCREEN_WIDTH: f32 = 800.0;

   // Scale step
   const SCALE_STEP: f32 = 0.3;

   const STATUS_BAR_HEIGHT: f32 = 30.0;

   pub fn new(world: Rc<RefCell<World>>) -> Self {
      let world_size = world.borrow().size();
      let world_size = Size2D::<usize, WorldSpace>::new(world_size.x, world_size.y);
      let world_size = world_size.cast::<f32>();

      // Scaling range
      let max_scale = ScaleWS::new(Self::MIN_SCREEN_WIDTH / Self::CELL_SIZE);   // about one cell on the screen
      let min_scale = ScaleWS::new(max_scale.get() / world_size.width);  // the whole world on the screen
   
      // Scale range, after down by 10% it is needs to up to (1-0.1^n)/(1-0.1)
      let scale_up = (1.0 - Self::SCALE_STEP.powi(10)) / (1.0 - Self::SCALE_STEP);
      let scale_down = 1.0 - Self::SCALE_STEP;

      let nonscaled_size = world_size * Self::CELL_SIZE;

      Self {
         life_cache: Cache::default(),
         grid_cache: Cache::default(),
         translation: PointW::default(),
         scale: ScaleWS::identity(),
         min_scale,
         max_scale,
         scale_up,
         scale_down,
         world,
         nonscaled_size,
         fps: RefCell::new(FPS::default()),
         tps: TPS::default(),
         illumination: false,
         last_tick: usize::MAX,
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

            self.life_cache.clear();
            self.grid_cache.clear();
         }

         Message::ClockChime => self.clock_chime(),
         Message::Illumination(is_on) => {
            self.illumination = is_on;
            self.life_cache.clear();
         }
         Message::FilterChanged => self.life_cache.clear(),
      }
   }


   pub fn view<'a>(&'a self) -> Element<'a, Message> {
      Canvas::new(self)
         .width(Length::Fill)
         .height(Length::Fill)
         .into()
   }

   fn adjust_translation(&self, translation: PointW) -> PointW {
      // To prevent overflow translation in coninious world
      let t = translation;
      let s = self.nonscaled_size;

      // Before PR to euclid
      // let x = ((t.x % s.width) + s.width) % s.width;
      // let y = ((t.y % s.height) + s.height) % s.height;
      // PointW::new(x, y)
      t.rem_euclid(&s)
   }

   fn translation_for_scale(&self, cursor: PointS, scale: ScaleWS) -> PointW {
      let offset = cursor / self.scale - cursor / scale;
      let offset = SizeW::new(offset.x, offset.y);

      self.translation + offset
}

// Update rate counters
   fn clock_chime(&mut self) {
      self.fps.borrow_mut().clock_chime();

      let tick = self.world.borrow().ticks_elapsed();
      self.tps.clock_chime(tick);

      // For update screen
      if self.last_tick != tick {
         self.last_tick = tick;
         self.life_cache.clear();
      }
   }

   fn draw_lines(&self, frame: &mut Frame, color: Color, step: SizeW, bounds: SizeS) {
      // Prepare style
      let stroke = Stroke::default()
      .with_width(1.0)
      .with_color(color);

      // Starting position with correction for a negative value
      // Before PR to euclid
      // let mut x = ((-self.translation.x % step.width) + step.width) % step.width * self.scale.get();
      // let mut y = ((-self.translation.y % step.height) + step.height) % step.height * self.scale.get();
      let p = (-self.translation).rem_euclid(&step);
      let mut p = self.scale.transform_point(p);
      let step = step * self.scale;

      // Draw vertical lines
      while p.x < bounds.width {
         let from = Point::new(p.x, 0.0);
         let to = Point::new(p.x, bounds.height);
         frame.stroke(&Path::line(from, to), stroke.to_owned());
         p.x += step.width;
      }

      // Draw horizontal lines
      while p.y < bounds.height {
         let from = Point::new(0.0, p.y);
         let to = Point::new(bounds.width, p.y);
         frame.stroke(&Path::line(from, to), stroke.to_owned());
         p.y += step.height;
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

      let cursor_position = if let Some(position) = cursor.position_in(&bounds) {
         PointS::new(position.x, position.y)
      } else {
         return (event::Status::Ignored, None);
      };

      match event {
         Event::Mouse(mouse_event) => match mouse_event {
            mouse::Event::ButtonPressed(button) => {
               let msg = match button {
                  mouse::Button::Right => {
                     *interaction = Interaction::Panning {
                           translation: self.translation,
                           start: cursor_position,
                     };
                     None
                  }
                  mouse::Button::Middle => {
                     // Choice of action - increase or decrease
                     let scale = if self.scale.get() > 10.0 * self.min_scale.get() {
                        self.min_scale
                     } else {
                        self.max_scale
                     };

                     let translation = self.translation_for_scale(cursor_position, scale);
                     Some(Message::Scaled(scale, Some(translation)))
                  }
                  _ => None,
               };

               (event::Status::Captured, msg)
            }

            mouse::Event::CursorMoved { .. } => {
               match *interaction {
                  Interaction::Panning { translation, start } => {
                     let offset = (cursor_position - start) / self.scale;
                     let offset = SizeW::new(offset.x, offset.y);
                     let translation = translation - offset;
                     let msg = Message::Translated(translation);

                     (event::Status::Captured, Some(msg))
                  }
                  Interaction::None => (event::Status::Ignored, None),
               }
            }
            mouse::Event::WheelScrolled { delta } => match delta {
               mouse::ScrollDelta::Lines { y, .. } | mouse::ScrollDelta::Pixels { y, .. } => {
                  if y < 0.0 && self.scale > self.min_scale || y > 0.0 && self.scale < self.max_scale {

                     let step = if y < 0.0 { self.scale_down } else { self.scale_up };
                     let scale = self.scale.get() * step;
                     let scale = scale.clamp(self.min_scale.get(), self.max_scale.get());
                     let scale = ScaleWS::new(scale);
                     let translation = self.translation_for_scale(cursor_position, scale);
                     (event::Status::Captured, Some(Message::Scaled(scale, Some(translation))))
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

      // Closure for draw world content
      let life_closure = |frame: &mut Frame| {

         let background = Path::rectangle(Point::ORIGIN, frame.size());
         frame.fill(&background, Color::BLACK);

         frame.scale(self.scale.get());
         let neg_iced_vector = Vector::new(-self.translation.x, -self.translation.y);
         frame.translate(neg_iced_vector);
         frame.scale(Self::CELL_SIZE);

         let frame_size: [f32; 2] = frame.size().into();
         let frame_size = SizeS::from(frame_size);

         // Ranges in region to draw
         let t = self.translation / Self::CELL_SIZE;
         let s: SizeW = frame_size / self.scale / Self::CELL_SIZE + SizeW::splat(1.0); // size in cells +1 for last incomplete cell
         let rx = t.x as isize .. (t.x + s.width) as isize;
         let ry = t.y as isize .. (t.y + s.height) as isize;

         // Closure for scaling
         let scaled_cell = self.scale.get() * Self::CELL_SIZE;
         let c = |i: isize| -> isize {
            (i as f32 * scaled_cell) as isize
         };

         // If frame is not large enough we need to exclude some cells
         let no_filter = s.width < frame_size.width;
         // Iterators skipping invisible points (skip, if the previous argument addresses the same point).
         let ix = rx.filter(|x| no_filter || c(*x - 1) != c(*x));
         let no_filter = s.height < frame_size.height;
         let iy = ry.filter(|y| no_filter || c(*y - 1) != c(*y));

         // The max number of lines of text to fit
         let pixels = self.scale.get() * Self::CELL_SIZE;
         let lines_number = if pixels > Self::CELL_SIZE_FOR_TEXT {
            (pixels / Self::CELL_TEXT_HEIGHT) as usize
         } else {0};

         // Draw each point
         // "iproduct" runs iterators repeatedly, we need to measure the performance compared to copying.
         for (x, y) in itertools::iproduct!(ix, iy) {
            // Get dot for point (allow display dot outside its real x and y)
            let dot = self.world.borrow().dot(x, y);
            let mut color = dot.color;
            if self.illumination {
               color.a = 1.0;
            }

            let p = Point::new(x as f32, y as f32);

            // Fill cell's area with a primary color
            frame.fill_rectangle(
               p,
               Size::UNIT,
               color,
            );

            // Draw the text if it fits
            if lines_number > 0 {
               let content = self.world.borrow().description(&dot, lines_number, '\n');
               let text = Text {
                  content,
                  position: p,
                  ..Text::default()
               };

               frame.with_save(|frame| {
                  frame.translate(Vector::new(0.03, 0.03));
                  frame.fill_text(text);
               });
            }
         };
      };

      let life = self.life_cache.draw(
         bounds.size(), |frame| {
            let region = Rectangle::with_size(bounds.size());
            frame.with_clip(region, |frame| life_closure(frame))
         }
      );

      let grid = self.grid_cache.draw(bounds.size(), |frame| {

         let bounds = SizeS::new(bounds.width, bounds.height);

         // Draw the inner grid if not too small scale
         if self.scale.get() * Self::CELL_SIZE > Self::MIN_VISIBLE_CELL_BORDER {
            let color = Color::from_rgb8(70, 74, 83);
            let step = SizeW::splat(Self::CELL_SIZE);
            self.draw_lines(frame, color, step, bounds);
         }

         // Draw outer borders - lines for border around the world
         let color = Color::from_rgb8(255, 74, 83);
         self.draw_lines(frame, color, self.nonscaled_size, bounds);
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
         let (years, days) = self.world.borrow().date();
         frame.fill_text(Text{
            position: Point::new(3.0, frame_height - 3.0),
            content: format!("{}Y:{}D {} FPS {} TPS", years, days, self.fps.borrow().rate, self.tps.rate),
            ..text
         });

         // Get dot below cursor
         if let Some(cursor_position) = cursor.position_in(&bounds) {

            let cursor = PointS::new(cursor_position.x, cursor_position.y);

            // Cursor at world coordinates
            let cursor = cursor / self.scale;
            let offset = SizeW::new(cursor.x, cursor.y);

            // Position at world coordinates
            let p = ((self.translation + offset) / Self::CELL_SIZE).floor();
            let top_left = Point::new(p.x, p.y);

            // Tune scale and offset
            frame.with_save(|frame| {
               frame.scale(self.scale.get());   // scale to user's choice
               let neg_iced_vector = Vector::new(-self.translation.x, -self.translation.y);
               frame.translate(neg_iced_vector);   // consider the offset of the displayed area
               frame.scale(Self::CELL_SIZE); // scale so that the cell with its dimensions occupies exactly one unit

               // Paint over a square of unit size
               frame.fill_rectangle(
                  top_left,
                  Size::UNIT,
                  Color {
                     a: 0.5,
                     ..Color::BLACK
                  },
               );
            });

            // Output info at bottom left edge
            let p = p.cast::<isize>();
            let dot = self.world.borrow().dot(p.x, p.y);
            let description = self.world.borrow().description(&dot, 30, ' ');
            frame.fill_text(Text{
               position: Point::new(210.0, frame_height - 3.0),
               content: format!("{}:{} {}", dot.x, dot.y, description),
               ..text
            });
         }

         frame.into_geometry()
      };

      // let handle = Handle::from_path(String::from("./res") + "/illuminate_on.png");
      // let v = iced::widget::image::viewer(handle);

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


pub enum Interaction {
   None,
   Panning { translation: PointW, start: PointS },
}

impl Default for Interaction {
   fn default() -> Self {
       Self::None
   }
}

