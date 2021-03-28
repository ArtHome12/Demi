/* ===============================================================================
Simulation of the evolution of the animal world.
Mechanism of evolution for the world.
03 Mar 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use crate::geom::*;
use crate::project::Project;

#[derive(Debug, Clone, Copy)]
pub struct Environment {
   // World size
   pub world_size: Size,

	// The maximum illumination radius around the location of the Sun is 90% of the height of the world
	pub light_radius: usize,

   // The course of the Sun vertically, due to the inclination of the earth axis, approximately sin (23.5) = 0.4 or 10% on each side of the equator
	pub tropic_height: f32,

   // Number of elements
   pub elements_count: usize,

   // How many bits to move (diffusion) elements in one tick
   pub num_points_to_diffuse: usize,

   // Range for serial number of all bits
   pub bits_count: usize,
}

impl Environment {
   // Number of ticks per day and days per year
   const TICKS_PER_DAY: f32 = 60.0*24.0;
   const DAYS_PER_YEAR: f32 = Self::TICKS_PER_DAY * 365.0;
   const HALF_YEAR: f32 = Self::DAYS_PER_YEAR / 2.0 + 0.5;

   pub fn new(project: &Project) -> Self {
      let element_count = project.elements.len();
      let world_size = project.size;
      let world_height = world_size.y;

      // How many bits to move (diffusion) elements in one tick
      let bits_count = world_size.x * world_height;
      let num_points_to_diffuse = (bits_count as f32 * project.resolution) as usize;

      Self {
         world_size,
         light_radius: (0.8 * world_height as f32 / 2.0) as usize,
         tropic_height: world_height as f32 / 5.0,
         elements_count: element_count,
         num_points_to_diffuse,
         bits_count,
      }
   }

   // Returns the position of the sun at the specified time
   pub fn sun_position(&self, tick: usize) -> Coord {
      // Times of Day
      let day_tick = tick as f32 % Self::TICKS_PER_DAY;

      let Size{x: w, y: h} = self.world_size;

      // The sun moves from east to west in proportion to the elapsed fraction of a day
      let x = w as f32 * (1.0 - day_tick / (Self::TICKS_PER_DAY - 1.0) + 0.5);

      // When the first half of the year goes, it is necessary to take a share from the equator, and when the second is to add
      let day = tick as f32 % Self::DAYS_PER_YEAR;
      let y = if day < Self::HALF_YEAR {
		   (h as f32 - self.tropic_height) / 2.0 + day * self.tropic_height / Self::HALF_YEAR
      } else {
		   (h as f32 + self.tropic_height) / 2.0 - (day - Self::HALF_YEAR) * self.tropic_height / Self::HALF_YEAR
      };

      Coord::new(x as usize, y as usize)
   }

   // Convert coordinates to serial number of bit
   pub fn serial(&self, x: usize, y: usize) -> usize {
      y * self.world_size.x + x
   }

   // Convert serial number of bit to coordinate pair x, y
   pub fn deserial(&self, serial: usize) -> (usize, usize) {
      let w = self.world_size.x;
      (serial % w, serial / w)
   }

   // Return index of the point from the given in the specified direction
   pub fn at_direction(&self, origin: usize, direction: Direction) -> usize {
      let (x, y) = self.deserial(origin);
      let Size {x : width, y: height} = self.world_size;

      let (x, y) = match direction {
         Direction::North => (
            x,
            if y > 0 {y - 1} else {height - 1}
         ),
         Direction::Northeast => (
            if x + 1 < width {x + 1} else {0},
            if y > 0 {y - 1} else {height - 1}
         ),
         Direction::East => (
            if x + 1 < width {x + 1} else {0},
            y
         ),
         Direction::Southeast => (
            if x + 1 < width {x + 1} else {0},
            if y + 1 < height {y + 1} else {0},
         ),
         Direction::South => (
            x,
            if y + 1 < height {y + 1} else {0},
         ),
         Direction::Southwest => (
            if x > 0 {x - 1} else {width - 1},
            if y + 1 < height {y + 1} else {0},
         ),
         Direction::West => (
            if x > 0 {x - 1} else {width - 1},
            y
         ),
         Direction::Northwest => (
            if x > 0 {x - 1} else {width - 1},
            if y > 0 {y - 1} else {height - 1}
         ),
      };

      self.serial(x, y)
   }
}