use std::rc::Rc;

pub struct TouchUI {}

impl TouchUI {
    pub fn new(core: Rc<core::Core>, dsp: Rc<dyn dsp::display::Display>) -> Self {
        Self {}
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {}
}
