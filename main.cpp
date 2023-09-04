#include "dsp/Dsp.h"
#include "core/Core.h"
#include "audio-midi/AudioMidi.h"
#include "ui/midi-ui/Ui.h"
#include "ui/touch-ui/Ui.h"

int main(int args, char** argv)
{
  Dsp::Dsp dsp;
  Core::Core core(dsp.getControlApi());
  AudioMidi::AudioMidi audioMidi(dsp.getRealtimeApi());
  Ui::Midi::Ui midiUI(core.getApi(), dsp.getDisplayApi());
  Ui::Touch::Ui touchUI(core.getApi(), dsp.getDisplayApi());
}