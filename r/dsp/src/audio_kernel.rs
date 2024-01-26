use std::rc::Rc;

pub type Sample = f32;
pub type SampleFrame = (Sample, Sample);
pub type SampleBuffer = Vec<SampleFrame>;
pub type SharedSampleBuffer = Rc<SampleBuffer>;

pub struct Tile {
    pub audio: Option<SharedSampleBuffer>,
    pub pattern: [bool; 64],
    pub gain_left: f32,
    pub gain_right: f32,
    pub playback_frame_increment: i8,
}

pub struct AudioKernel {
    pub volume: f32,
    pub frames_per16th: u32,
    pub tiles: [Tile; 36],
}

impl AudioKernel {}
