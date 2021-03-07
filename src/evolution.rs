/* ===============================================================================
Simulation of the evolution of the animal world.
Mechanism of evolution for the world.
03 Mar 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::thread::{self, JoinHandle};
use std::sync::{Arc, atomic::{Ordering, AtomicBool}};

use crate::{dot::{/*Bit,*/ Bits,}};

pub struct Evolution {
   // Thread for calculate evolution and control flag
   thread: JoinHandle<()>,
   run_flag: Arc<AtomicBool>, // running when true else paused
}

impl Evolution {
   pub fn new(bits: &Bits) -> Self {
      // Flags for thread control
      let run_flag = Arc::new(AtomicBool::new(false));
      let run_flag_threaded = Arc::clone(&run_flag);

      // Thread for calculate evolution
      let thread = thread::spawn(move || {
         // Running until program not closed
         loop {

            // Sleep if it paused
            if !run_flag_threaded.load(Ordering::Acquire) {
               thread::park();
            }

            // Calculate the tick of evolution
            Self::make_tick();
         }
      });

      Self {
         thread,
         run_flag,
      }
   }

   fn make_tick() {

   }

   pub fn toggle_pause(&self) {
      // Transfer signal to thread
      let flag = !self.run_flag.load(Ordering::Acquire);
      self.run_flag.store(flag, Ordering::Release);

      // If it has been suspended, wake up
      if flag {self.thread.thread().unpark()}
   }
}
