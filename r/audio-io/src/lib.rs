use std::rc::Rc;

pub struct AlsaAudioOut
{
    device: String,
    dsp: Rc<dyn dsp::Realtime>,
}

pub trait AudioOut {
    fn new(device: String, dsp: Rc<dyn dsp::Realtime>) -> Self;
    fn run(&self);
}


impl AudioOut for AlsaAudioOut {
    fn new(device: String, dsp: Rc<dyn dsp::Realtime>) -> Self {
        Self { device, dsp: dsp }
    }

    fn run(&self)
    {
        self.dsp.do_audio();
    }
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {}
}
