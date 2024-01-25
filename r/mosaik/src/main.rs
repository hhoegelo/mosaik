use audio_io::AudioOut;
use dsp;

fn main() {
    let mut dsp = dsp::Dsp::new();
    let audio_out = audio_io::AlsaAudioOut::new("".to_string(), dsp.clone());
    let mut core = core::Core::new(dsp.clone());
    let touch_ui = touch_ui::TouchUI::new(core.clone(), dsp.clone());

    audio_out.run();
}
