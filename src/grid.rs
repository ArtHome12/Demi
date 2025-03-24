/* ===============================================================================
Simulation of the evolution of the animal world.
Grid of world.
14 Feb 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2023 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use iif::iif;

use iced::{alignment, Color, Element, Length, Point, Rectangle, Size, Theme, Vector,
   Transformation, Renderer,
};
use iced::mouse::{self, Cursor,};
use iced::widget::{image::Handle, Image, image, stack,};
use iced::widget::canvas::{self, Cache, Canvas, Event, Frame, Geometry, Path, Stroke, Text, };
use iced::advanced::renderer;
use iced::advanced::image::Bytes;
use iced::advanced::widget::{self, Widget};
use iced::advanced::layout::{self, Layout};
use iced::advanced::graphics::color;
use iced::advanced::graphics::mesh::{self, SolidVertex2D,};

use std::{rc::Rc, cell::RefCell, ptr};
use euclid::*;

use crate::world::World;
use crate::update_rate::*;

pub struct Grid {
   cell_text_cache: Cache,
   grid_cache: Cache,
   translation: PointW,
   bitmap: RefCell<Bitmap>,   // to store the off-screen image used at small scale for performance
   mesh: RefCell<MeshWidget>,

   scale: ScaleWS,
   min_scale: ScaleWS,
   max_scale: ScaleWS,
   scale_up: ScaleSS,
   scale_down: ScaleSS,

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
   ClockChime(bool), // Update screen and rate counters if true
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
type ScaleSS = Scale<f32, ScreenSpace, ScreenSpace>;
type LenS = euclid::Length<f32, ScreenSpace>;
type LenW = euclid::Length<f32, WorldSpace>;

impl Grid {
   // Default size of one cell
   const CELL_SIZE: LenW = LenW::new(30.0);

   // Minimum size to draw a text
   const CELL_SIZE_FOR_TEXT: LenS = LenS::new(120.0);

   // Height for line of text
   const CELL_TEXT_HEIGHT: LenS = LenS::new(21.0);

   // If less, cell borders are not drawn
   const MIN_VISIBLE_CELL_BORDER: LenS = LenS::new(5.0);

   // Screen size defines scaling range
   const MIN_SCREEN_WIDTH: LenW = LenW::new(800.0);

   // Scale step
   const SCALE_STEP: f32 = 0.3;

   const STATUS_BAR_HEIGHT: f32 = 30.0;


   pub fn new(world: Rc<RefCell<World>>) -> Self {
      let world_size = world.borrow().size();
      let world_size = Size2D::<usize, WorldSpace>::new(world_size.x, world_size.y);
      let world_size = world_size.cast::<f32>();

      // Scaling range
      let r = Self::MIN_SCREEN_WIDTH / Self::CELL_SIZE;
      let max_scale = ScaleWS::new(r.get());   // about one cell on the screen
      let min_scale = ScaleWS::new(r.get() / world_size.width);  // the whole world on the screen
   
      // Scale range, after down by 10% it is needs to up to (1-0.1^n)/(1-0.1)
      let scale_up = ScaleSS::new((1.0 - Self::SCALE_STEP.powi(10)) / (1.0 - Self::SCALE_STEP));
      let scale_down = ScaleSS::new(1.0 - Self::SCALE_STEP);

      let nonscaled_size = world_size * Self::CELL_SIZE.get();

      Self {
         cell_text_cache: Cache::default(),
         grid_cache: Cache::default(),
         translation: PointW::default(),
         bitmap: RefCell::new(Bitmap::new()),
         mesh: RefCell::new(MeshWidget::new()),
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
            self.clear_cache(true);
         }

         Message::Scaled(scale, translation) => {
            self.scale = scale;

            if let Some(translation) = translation {
               self.translation = self.adjust_translation(translation);
            }
            self.clear_cache(true);
         }

         Message::ClockChime(is_one_second_passed) => self.clock_chime(is_one_second_passed),
         Message::Illumination(is_on) => {
            self.illumination = is_on;
            self.clear_cache(false);
         }
         Message::FilterChanged => self.clear_cache(false),
      }
   }


   // Force to redraw the cells and grid
   fn clear_cache(&mut self, with_grid: bool) {
      self.cell_text_cache.clear();
      self.bitmap.borrow_mut().clear();
      self.mesh.borrow_mut().clear();
      if with_grid {
         self.grid_cache.clear();
      }
   }


   pub fn view(&self, size: iced::Size) -> Element<Message> {
      let canvas = Canvas::new(self)
         .width(Length::Fill)
         .height(Length::Fill)
         .into();
      
      let size = SizeS::new(size.width, size.height);

      if self.use_bitmap() {
         let bitmap = self.bitmap.borrow_mut()
            .update(size, self);
         let img: Element<Message> =Element::from(bitmap);

         stack(vec![img, canvas,]).into()
      } else {
         self.mesh.borrow_mut().generate(size, self);
         let geom = self.mesh.borrow().clone().into();
         stack(vec![geom, canvas,]).into()
      }
   }


   fn adjust_translation(&self, translation: PointW) -> PointW {
      // To prevent overflow translation in coninious world
      let t = translation;
      let s = self.nonscaled_size;
      t.rem_euclid(&s)
   }


   fn translation_for_scale(&self, cursor: PointS, scale: ScaleWS) -> PointW {
      let offset = cursor / self.scale - cursor / scale;
      let offset = SizeW::new(offset.x, offset.y);

      self.translation + offset
   }


   // Update rate counters
   fn clock_chime(&mut self, is_one_second_passed: bool) {

      // Current model time
      let tick = self.world.borrow().ticks_elapsed();

      // Update FPS after a second has passed
      if is_one_second_passed {
         self.fps.borrow_mut().clock_chime();

         self.tps.clock_chime(tick);
      };

      // For update screen
      if self.last_tick != tick {
         self.last_tick = tick;
         self.clear_cache(false)
      }
      canvas::Action::<Message>::request_redraw();
   }


   /// Draws vertical and horizontal lines with the specified step. Used for cell and world borders.
   fn draw_lines(&self, frame: &mut Frame, color: Color, step: SizeW, bounds: SizeS) {
      // Prepare style
      let stroke = Stroke::default()
      .with_width(1.0)
      .with_color(color);

      // Starting position with correction for a negative value
      let p = (-self.translation).rem_euclid(&step);
      let mut p = p * self.scale;
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


   // If true, use bitmap for points drawing else use canvas
   fn use_bitmap(&self) -> bool {
      Self::CELL_SIZE * self.scale < Self::MIN_VISIBLE_CELL_BORDER
   }
}

impl<'a> canvas::Program<Message> for Grid {

   type State = Interaction;

   fn update(
      &self,
      interaction: &mut Interaction,
      event: &Event,
      bounds: Rectangle,
      cursor: Cursor,
   ) -> Option<canvas::Action<Message>> {
      if let Event::Mouse(mouse::Event::ButtonReleased(_)) = event {
         *interaction = Interaction::None;
      }

      let cursor_position = if let Some(position) = cursor.position_in(bounds) {
         PointS::new(position.x, position.y)
      } else {
         return None;
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
                     let scale = iif!(self.scale.get() > 10.0 * self.min_scale.get(), self.min_scale, self.max_scale);

                     let translation = self.translation_for_scale(cursor_position, scale);
                     Some(Message::Scaled(scale, Some(translation)))
                  }
                  _ => None,
               };

               Some(
                  msg
                      .map(canvas::Action::publish)
                      .unwrap_or(canvas::Action::request_redraw())
                      .and_capture(),
              )
            }

            mouse::Event::CursorMoved { .. } => {
               let msg = match *interaction {
                  Interaction::Panning { translation, start } => {
                     let offset = (cursor_position - start) / self.scale;
                     let offset = SizeW::new(offset.x, offset.y);
                     let translation = translation - offset;
                     Some(Message::Translated(translation))
                  }
                  Interaction::None => None,
               };

               let action = msg
               .map(canvas::Action::publish)
               .unwrap_or(canvas::Action::request_redraw());

               Some(match interaction {
                     Interaction::None => action,
                     _ => action.and_capture(),
               })
            }
            mouse::Event::WheelScrolled { delta } => match *delta {
               mouse::ScrollDelta::Lines { y, .. } | mouse::ScrollDelta::Pixels { y, .. } => {
                  let is_inside = y < 0.0 && self.scale > self.min_scale 
                     || y > 0.0 && self.scale < self.max_scale;

                  let action = if is_inside {
                     let step = iif!(y < 0.0, self.scale_down, self.scale_up);
                     let scale = self.scale * step;
                     let scale = scale.clamp(self.min_scale, self.max_scale);
                     let translation = self.translation_for_scale(cursor_position, scale);
                     
                     canvas::Action::publish(Message::Scaled(
                           scale,
                           Some(translation),
                     ))
                     .and_capture()
                  } else {
                     canvas::Action::capture()
                  };

                  Some(action)
               }
            },
            _ => None,
         },
         _ => None,
      }
   }

   
   fn draw(
      &self,
      _interaction: &Interaction,
      renderer: &Renderer,
      theme: &Theme,
      bounds: Rectangle,
      cursor: Cursor,
   ) -> Vec<Geometry> {

      // Update FPS, once upon refresh
      self.fps.borrow_mut().make_tick();

      // The maximum number of lines of text that fit in a cell
      let cell_scaled_size = Self::CELL_SIZE * self.scale;
      let lines = if cell_scaled_size > Self::CELL_SIZE_FOR_TEXT {
         (cell_scaled_size / Self::CELL_TEXT_HEIGHT).get() as usize
      } else {0};

      // Draw text inside cells if it fits
      let cell_text = if lines > 0 {

         let closure = |frame: &mut Frame| {

            frame.scale(self.scale.get());
            let neg_iced_vector = Vector::new(-self.translation.x, -self.translation.y);
            frame.translate(neg_iced_vector);

            // Scale for text inside cells
            let text_scale = 1.0 / self.scale.get();
            frame.scale(text_scale);
            
            let s = &frame.size();
            let frame_size = SizeS::new(s.width, s.height);
            let s: SizeW = frame_size / self.scale / Self::CELL_SIZE.get() + SizeW::splat(1.0); // size in cells +1 for last incomplete cell

            // Ranges in region to draw
            let t = self.translation / Self::CELL_SIZE.get();
            let rx = t.x as isize .. (t.x + s.width) as isize;
            let ry = t.y as isize .. (t.y + s.height) as isize;

            // Draw the text if it fits
            let world = self.world.borrow();
            // "iproduct" runs iterators repeatedly, we need to measure the performance compared to copying.
            for (x, y) in itertools::iproduct!(rx, ry) {
               // Get dot for point (allow display dot outside its real x and y)
               let dot = world.dot(x, y);
               let mut color = dot.color;
               if self.illumination {
                  color.a = 1.0;
               }

               let content = world.description(&dot, lines, '\n');
               let s = cell_scaled_size.get();
               let position = Point::new(x as f32 * s + 3.0, y as f32 * s); // with an indent from the left edge

               // Contrast color
               let avg_color = (color.r * color.a + color.g * color.a + color.b * color.a) / 3.0;
               let color = if avg_color < 0.2 { Color::from_rgb8(210, 210, 210) }
                  else if avg_color < 0.5 { Color::WHITE }
                  else { Color::BLACK };

               let text = Text {
                  content,
                  position,
                  color,
                  ..Text::default()
               };

               frame.fill_text(text);
            }
         };

         // Return points on the canvas
         let geom = self.cell_text_cache.draw(renderer,
            bounds.size(), |frame| {
               let region = Rectangle::with_size(bounds.size());
               frame.with_clip(region, |frame| closure(frame))
            }
         );
         Some(geom)
      } else {
         None
      };

      let grid = self.grid_cache.draw(renderer,
         bounds.size(),
         |frame| {
            let bounds = SizeS::new(bounds.width, bounds.height);

            // Draw the inner grid if not too small scale - similar with using bitmap.
            if !self.use_bitmap() {
               let color = Color::from_rgb8(70, 74, 83);
               let step = SizeW::splat(Self::CELL_SIZE.get());
               self.draw_lines(frame, color, step, bounds);
            }

            // Draw outer borders - lines for border around the world
            self.draw_lines(frame, theme.palette().text, self.nonscaled_size, bounds);
         }
      );

      let overlay = {
         let mut frame = Frame::new(renderer, bounds.size());

         // Translucent bar at the bottom of the window
         let frame_width = frame.width();
         let frame_height = frame.height();
         frame.fill_rectangle(
            Point::new(0.0, frame_height - Self::STATUS_BAR_HEIGHT),
            Size::new(frame_width, Self::STATUS_BAR_HEIGHT),
            Color {
               a: 0.9,
               ..theme.palette().background
            }
         );

         // Text object
         let text = Text {
            color: theme.palette().text,
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
         if let Some(cursor_position) = cursor.position_in(bounds) {

            let cursor = PointS::new(cursor_position.x, cursor_position.y);

            // Cursor at world coordinates
            let cursor = cursor / self.scale;
            let offset = SizeW::new(cursor.x, cursor.y);

            // Position at world coordinates
            let p = ((self.translation + offset) / Self::CELL_SIZE.get()).floor();

            // Draw a square over the cell if not in bitmap mode
            if !self.use_bitmap() {
               frame.with_save(|frame| {
                  // Tune scale and offset
                  frame.scale(self.scale.get());   // scale to user's choice
                  let neg_iced_vector = Vector::new(-self.translation.x, -self.translation.y);
                  frame.translate(neg_iced_vector);   // consider the offset of the displayed area
                  frame.scale(Self::CELL_SIZE.get()); // scale so that the cell with its dimensions occupies exactly one unit

                  // Paint over a square of unit size
                  let top_left = Point::new(p.x, p.y);
                  frame.fill_rectangle(
                     top_left,
                     Size::UNIT,
                     Color {
                        a: 0.5,
                        ..Color::BLACK
                     },
                  )
               })
            }

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

      match cell_text {
         Some(life) => vec![life, grid, overlay],
         None => vec![grid, overlay]
      }
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

// To store the off-screen image
struct Bitmap {
   capacity: usize,
   cached: bool,
   storage: Vec<u8>,
}

impl Bitmap {
   fn new() -> Self {
      Self {
         capacity: 0,   // capacity as criterion for cache
         cached: false,
         storage: Vec::new(),
      }
   }

   fn update(&mut self, size: SizeS, grid: &Grid) -> Image {

      // Size in cells, maybe it is necessary +1 for last incomplete column and row.
      let s: SizeW = size / grid.scale / Grid::CELL_SIZE.get(); 

      // Calculate new capacity. "Size" may be less than "s" due to filtering of points in the iterator.
      let width = s.width.min(size.width) as usize;
      let height = s.height.min(size.height) as usize;
      let capacity = width * height * 4;

      // If the size has changed, then the cache is not valid
      self.cached = self.cached && (self.capacity == capacity);
      self.capacity = capacity;

      // Recreate the storage when the size changes
      if self.storage.capacity() < capacity {
         self.storage = vec![0; capacity];
      }
      
      // Build the image
      if !self.cached {
         self.cached = true;

         // Ranges in region to draw
         let t = grid.translation / Grid::CELL_SIZE.get();
         let rx = t.x as isize .. (t.x as isize + s.width as isize);
         let ry = t.y as isize .. (t.y as isize + s.height as isize);
   
         // Closure for scaling
         let scaled_cell = Grid::CELL_SIZE * grid.scale;
         let c = |i: isize| -> isize {
            (i as f32 * scaled_cell.get()) as isize
         };
   
         // If frame is not large enough we need to exclude some cells
         let no_filter = s.width < size.width;
         // Iterators skipping invisible points (skip, if the previous argument addresses the same point).
         let ix = rx.filter(|x| no_filter || c(*x - 1) != c(*x));
         let no_filter = s.height < size.height;
         let iy = ry.filter(|y| no_filter || c(*y - 1) != c(*y));
   
         // Draw each point
         let world = grid.world.borrow();
         let mut i = 0; // point index in storage
         for (y, x) in itertools::iproduct!(iy, ix) {
            // Get dot for point (allow display dot outside its real x and y)
            let dot = world.dot(x, y);
            let mut color = dot.color;
            if grid.illumination {
               color.a = 1.0;
            }
   
            let color = Vec::from(color.into_rgba8());
            unsafe {
               let dst = self.storage.as_mut_ptr().add(i);
               ptr::copy_nonoverlapping(color.as_ptr(), dst, 4);
            }
            i += 4;
         };
      }

      // Return the image
      let cnt = self.storage.len();
      let pixels = Bytes::from(self.storage.clone());
      let height = cnt / width / 4; // the formula is sometimes wrong by one
      let handle = Handle::from_rgba(width as u32, height as u32, pixels);

      // Only increase, the decrease is taken into account through the size.
      let scale = (grid.scale.get() * Grid::CELL_SIZE.get()).max(1.0);

      image(handle)
         .width(size.width)   // reserved space for the Element, does not affect the display of the image
         .height(size.height)
         .scale(scale)
         .filter_method(image::FilterMethod::Nearest)
         .content_fit(iced::ContentFit::None)
   }


   fn clear(&mut self) {
      self.cached = false;
   }
}

#[derive(Debug, Clone)]
struct MeshWidget {
   capacity: usize,  // capacity of vertices as criterion for cache
   cached: bool,
   vertices: Vec<SolidVertex2D>,
   indices: Vec<u32>,
}

impl MeshWidget {
   fn new() -> Self {
      Self {
         capacity: 0,
         cached: false,
         vertices: Vec::new(),
         indices: Vec::new(),
      }
   }


   fn generate(&mut self, size: SizeS, grid: &Grid) {

      // Size in cells +1 for last incomplete column and row and +1 due to translation.
      let s: SizeW = size / grid.scale / Grid::CELL_SIZE.get() + SizeW::splat(1.0);

      // Calculate new capacity.
      let capacity = (s.width * s.height) as usize;

      // If the size has changed, then the cache is not valid
      self.cached = self.cached && (self.capacity == capacity);
      self.capacity = capacity;

      // Build the mesh
      if !self.cached {
         self.cached = true;

         // Clear the storage
         self.vertices.clear();
         self.indices.clear();

         // Ranges in region to draw
         let t = grid.translation / Grid::CELL_SIZE.get();
         let rx = t.x as isize .. (t.x + s.width) as isize;
         let ry = t.y as isize .. (t.y + s.height) as isize;

         // Go through all the points
         let scale1 = Grid::CELL_SIZE.get();
         let scale2 = grid.scale.get();
         let world = grid.world.borrow();
         for (y, x) in itertools::iproduct!(ry, rx) {
            // Get dot for point (allow display dot outside its real x and y)
            let dot = world.dot(x, y);
            let mut color = dot.color;
            if grid.illumination {
               color.a = 1.0;
            }

            let color = color::pack(color);

            let x0 = (x as f32 * scale1 - grid.translation.x) * scale2;
            let y0 = (y as f32 * scale1 - grid.translation.y) * scale2;
            let x1 = ((x + 1) as f32 * scale1 - grid.translation.x) * scale2;
            let y1 = y0;
            let x2 = x0;
            let y2 = ((y + 1) as f32 * scale1 - grid.translation.y) * scale2;
            let x3 = x1;
            let y3 = y2;

            // Transform to vetex and store
            self.vertices.push(SolidVertex2D {position: [x0, y0], color});
            self.vertices.push(SolidVertex2D {position: [x1, y1], color});
            self.vertices.push(SolidVertex2D {position: [x2, y2], color});
            self.vertices.push(SolidVertex2D {position: [x3, y3], color});
         };

         // Create triange edges and store with closure
         let c_store_indice = |i: usize| {
               self.indices.push(i as u32);
         };

         // Use len() because width * height not equal to math in f32
         Self::indices(c_store_indice, self.vertices.len());
      }
   }

   
   // A set of indices of triangle edges.
   fn indices<F>(mut c_store_indice: F, count: usize) where F: FnMut(usize) {
      for i in (0..count).step_by(4) {
         c_store_indice(i);
         c_store_indice(i + 1);
         c_store_indice(i + 2);

         c_store_indice(i + 1);
         c_store_indice(i + 2);
         c_store_indice(i + 3);
      };
   }

   
   fn clear(&mut self) {
      self.cached = false;
   }
}

impl<Message> Widget<Message, Theme, Renderer> for MeshWidget {
   fn size(&self) -> Size<Length> {
       Size {
           width: Length::Fill,
           height: Length::Fill,
       }
   }

   fn layout(
       &self,
       _tree: &mut widget::Tree,
       _renderer: &Renderer,
       limits: &layout::Limits,
   ) -> layout::Node {
       layout::Node::new(limits.max())
   }

   fn draw(
       &self,
       _tree: &widget::Tree,
       renderer: &mut Renderer,
       _theme: &Theme,
       _style: &renderer::Style,
       layout: Layout<'_>,
       _cursor: mouse::Cursor,
       _viewport: &Rectangle,
   ) {
       use iced::advanced::Renderer as _;
       use iced::advanced::graphics::mesh::{
           Mesh, Renderer as _,
       };

       // Mesh must not have empty indices
       if self.indices.is_empty() {
           return;
       }
       
       // In client area
       let bounds = layout.bounds();

       // Without project controls above
       let clip_bounds = Rectangle::with_size(bounds.size());

       let mesh = Mesh::Solid {
         buffers: mesh::Indexed {
             vertices: self.vertices.clone(),
             indices: self.indices.clone(),
         },
         transformation: Transformation::IDENTITY,
         clip_bounds: clip_bounds,
      };

      renderer.with_translation(
         Vector::new(bounds.x, bounds.y),
         |renderer| {
            renderer.draw_mesh(mesh);
         },
       );
   }
}


impl<Message> From<MeshWidget> for Element<'_, Message> {
   fn from(mesh_widget: MeshWidget) -> Self {
       Self::new(mesh_widget)
   }
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
         let width = 4;
         let height = 3;

         // let verticles = vec![
         //    [0, 0], [1, 0], [0, 1], [1, 1],
         //    [1, 0], [2, 0], [1, 1], [2, 1],
         //    [2, 0], [3, 0], [2, 1], [3, 1],
         //    [3, 0], [4, 0], [3, 1], [4, 1],
         //    [0, 1], [1, 1], [0, 2], [1, 2],
         //    [1, 1], [2, 1], [1, 2], [2, 2], 
         //    [2, 1], [3, 1], [2, 2], [3, 2],
         //    [3, 1], [4, 1], [3, 2], [4, 2],
         //    [0, 2], [1, 2], [0, 3], [1, 3],
         //    [1, 2], [2, 2], [1, 3], [2, 3],
         //    [2, 2], [3, 2], [2, 3], [3, 3],
         //    [3, 2], [4, 2], [3, 3], [4, 3],
         // ];
 
         // Test for MeshWidget::indices
         let mut indices = Vec::<u32>::new();
         MeshWidget::indices(|i: usize| {indices.push(i as u32)}, width * height * 4);
         let answer = vec![
            0, 1, 2, 1, 2, 3,
            4, 5, 6, 5, 6, 7,
            8, 9, 10, 9, 10, 11,
            12, 13, 14, 13, 14, 15,
            16, 17, 18, 17, 18, 19,
            20, 21, 22, 21, 22, 23,
            24, 25, 26, 25, 26, 27,
            28, 29, 30, 29, 30, 31,
            32, 33, 34, 33, 34, 35,
            36, 37, 38, 37, 38, 39,
            40, 41, 42, 41, 42, 43,
            44, 45, 46, 45, 46, 47];
         assert_eq!(indices, answer, "indices");
    }
}