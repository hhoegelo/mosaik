#pragma once

#include <ui/midi-ui/Types.h>
#include <functional>
#include <midi/Alsa.h>

namespace Ui::Midi
{
  class MidiEventDispatcher
  {
   public:
    MidiEventDispatcher(std::function<void(SoftButton, ButtonEvent)> softButtonEvent,
                        std::function<void(Step, ButtonEvent)> stepButtonEvent,
                        std::function<void(Knob, int)> knobEvent);

    void dispatch(const ::Midi::Alsa::MidiEvent &event) const;

   private:
    std::function<void(SoftButton, ButtonEvent)> m_softButtonEvent;
    std::function<void(Step, ButtonEvent)> m_stepButtonEvent;
    std::function<void(Knob, int)> m_knobEvent;
  };
}
