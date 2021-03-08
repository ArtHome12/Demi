/* ===============================================================================
Simulation of the evolution of the animal world.
Mechanism of evolution for the world.
03 Mar 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

use std::{thread::{self, JoinHandle}};
use std::sync::{Arc, atomic::{Ordering, AtomicBool, AtomicUsize,}};

use crate::{dot::{/*Bit,*/ Bits,}};

pub struct Evolution {
   thread: JoinHandle<()>, // thread for calculate evolution and control flag
   run_flag: Arc<AtomicBool>, // running when true else paused
   ticks_elapsed: Arc<AtomicUsize>, // model time - a number ticks elapsed from beginning
}

impl Evolution {
   pub fn new(bits: &Bits) -> Self {
      // Flags for thread control
      let run_flag = Arc::new(AtomicBool::new(false));
      let run_flag_threaded = Arc::clone(&run_flag);

      // The model time
      let ticks_elapsed = Arc::new(AtomicUsize::new(0));
      let ticks_elapsed_threaded = Arc::clone(&ticks_elapsed);

      // Thread for calculate evolution
      let thread = thread::spawn(move || {
         // Running until program not closed
         loop {

            // Sleep if it paused
            if !run_flag_threaded.load(Ordering::Acquire) {
               thread::park();
            }

            // Calculate the tick of evolution
            ticks_elapsed_threaded.fetch_add(1, Ordering::Relaxed);
            Self::make_tick();
         }
      });

      Self {
         thread,
         run_flag,
         ticks_elapsed,
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

   // Returns model time - a number ticks elapsed from beginning
   pub fn ticks_elapsed(&self) -> usize {
      self.ticks_elapsed.load(Ordering::Relaxed)
   }
}
