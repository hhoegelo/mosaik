use std::{ptr, rc::Rc};

pub use audio_kernel::AudioKernel;
use transfer_ptr::TransferPointer;

mod audio_kernel;
pub mod control;
pub mod display;
pub mod realtime;

pub use control::Control;
pub use display::Display;
pub use realtime::Realtime;

mod transfer_ptr;

pub type TileId = u8;

pub struct Dsp {
    _kernel: TransferPointer<AudioKernel>,
}

impl Dsp {
    pub fn new() -> Rc<Self> {
        Rc::new(Self {
            _kernel: TransferPointer::new(ptr::null_mut()),
        })
    }
}

impl control::Control for Dsp {
    fn take_audio_kernel(&self, _kernel: audio_kernel::AudioKernel) {
        todo!()
    }
}

impl realtime::Realtime for Dsp {
    fn do_audio(&self) {
        todo!()
    }
}

impl display::Display for Dsp {
    fn is_playing(&self, _tile_id: TileId) {
        todo!()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {}
}
