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
    constexpr auto noteOn = 0x90;
    constexpr auto noteOff = 0x80;

    if(event[0] == noteOn)
    {
      printf("Note on...\n");

      if(event[1] < 64)
      {
        printf("for step %d => Button Press\n", event[1]);
        m_stepButtonEvent(static_cast<Step>(event[1]), ButtonEvent::Press);
      }
      else
      {
        printf("for button %d => Button Press\n", event[1]);
        m_softButtonEvent(static_cast<SoftButton>(event[1]), ButtonEvent::Press);
      }
    }
    else if(event[0] == noteOff)
    {
      printf("Note off...\n");

      if(event[1] < 64)
      {
        printf("for step %d => Button Release\n", event[1]);
        m_stepButtonEvent(static_cast<Step>(event[1]), ButtonEvent::Release);
      }
      else
      {
        printf("for button %d => Button Release\n", event[1]);
        m_softButtonEvent(static_cast<SoftButton>(event[1]), ButtonEvent::Release);
      }
    }
    else if(event[0] == 0xb0)
    {
      printf("knob inc %d for knob %d\n", static_cast<int>(event[2]) - 64, event[1]);
      m_knobEvent(static_cast<Knob>(event[1]), static_cast<int>(event[2]) - 64);
    }
  }
}