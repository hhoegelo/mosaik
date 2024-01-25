use crate::audio_kernel::AudioKernel;

pub trait Control {
    fn take_audio_kernel(&self, kernel: AudioKernel);
}
