#include <memory>
#include <utility>
#include "Ui.h"
#include <midi/Monitor.h>
#include <midi/Alsa.h>
#include <glibmm.h>

namespace Ui::Midi
{

  Ui::Ui(SharedState &sharedUiState, std::string midiDevice, Core::Api::Interface &core,
         Dsp::Api::Display::Interface &dsp)
      : m_midiDevice(std::move(midiDevice))
      , m_monitor(std::make_unique<::Midi::Monitor>())
      , m_timer(Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &Ui::checkForMidiDevices), 1))
      , m_controller(sharedUiState, core, dsp, *this)
      , m_dispatcher([this](SoftButton btn, ButtonEvent e) { m_controller.onSoftButtonEvent(btn, e); },
                     [this](Step btn, ButtonEvent e) { m_controller.onStepButtonEvent(btn, e); },
                     [this](Knob k, int inc) { m_controller.onErpInc(k, inc); })
  {
    m_controller.kickOff();
  }

  bool Ui::checkForMidiDevices()
  {
    m_monitor->poll(
        [this](auto &foundDevice)
        {
          if(foundDevice == m_midiDevice)
            m_inputDevices[foundDevice] = std::make_unique<::Midi::Alsa>(
                foundDevice, [this](const ::Midi::Alsa::MidiEvent &event) { m_dispatcher.dispatch(event); });
        },
        [this](auto lostDevice) { m_inputDevices.erase(lostDevice); });
    return true;
  }

  Ui::~Ui()
  {
    m_timer.disconnect();
  }

  void Ui::highlightCurrentStep(Step oldStep, Step newStep)
  {
    setStepButtonColor(oldStep, Color::None);
    setStepButtonColor(oldStep, Color::Blue);
  }

  void Ui::setStepButtonColor(Step step, Color c)
  {
    auto sendChannel = [&](auto device, Color c, uint8_t brightness)
    {
      auto b = static_cast<uint8_t>(0x90 | static_cast<uint8_t>(c));
      device->send({ b, step, brightness });
    };

    for(auto &a : m_inputDevices)
    {
      if(c == Color::None)
      {
        sendChannel(a.second.get(), Color::Red, 0);
        sendChannel(a.second.get(), Color::Green, 0);
        sendChannel(a.second.get(), Color::Blue, 0);
        sendChannel(a.second.get(), Color::White, 0);
      }
      else
      {
        sendChannel(a.second.get(), Color::White, 100);
      }
    }
  }

  void Ui::setSoftButtonColor(SoftButton button, Color c)
  {
  }
}
