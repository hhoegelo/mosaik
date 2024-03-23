#include "MidiEventDispatcher.h"

namespace Ui::Midi
{
  MidiEventDispatcher::MidiEventDispatcher(std::function<void(SoftButton, ButtonEvent)> softButtonEvent,
                                           std::function<void(Step, ButtonEvent)> stepButtonEvent,
                                           std::function<void(Knob, int)> knobEvent)
      : m_softButtonEvent(std::move(softButtonEvent))
      , m_stepButtonEvent(std::move(stepButtonEvent))
      , m_knobEvent(std::move(knobEvent))
  {
  }

  void MidiEventDispatcher::dispatch(const ::Midi::Alsa::MidiEvent &event) const
  {
    if(event[0] == 0x90)
    {
      if(event[1] < 64)
      {
        if(event[2] == 1)
        {
          m_stepButtonEvent(static_cast<Step>(event[1]), ButtonEvent::Press);
          // fake it as long as HW does not send the release
          m_stepButtonEvent(static_cast<Step>(event[1]), ButtonEvent::Release);
        }
      }
      else
      {
        if(event[2] == 1)
        {
          m_softButtonEvent(static_cast<SoftButton>(event[1]), ButtonEvent::Press);
          // fake it as long as HW does not send the release
          m_softButtonEvent(static_cast<SoftButton>(event[1]), ButtonEvent::Release);
        }
      }
    }
    else if(event[0] == 0xb0)
    {
      m_knobEvent(static_cast<Knob>(event[1]), 5 * (static_cast<int>(event[2]) - 64));
    }

    printf("%x %d %d\n", event[0], event[1], event[2]);
  }
}