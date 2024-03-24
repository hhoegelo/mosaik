#include <memory>
#include <utility>
#include "Ui.h"
#include <midi/Monitor.h>
#include <midi/Alsa.h>
#include <glibmm.h>

namespace Ui::Midi
{

  Ui::Ui(std::string midiDevice, Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp,
         ::Ui::Touch::Interface &touchUi)
      : m_midiDevice(std::move(midiDevice))
      , m_monitor(std::make_unique<::Midi::Monitor>())
      , m_timer(Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &Ui::checkForMidiDevices), 1))
      , m_controller(core, dsp, touchUi, *this)
      , m_dispatcher([this](SoftButton btn, ButtonEvent e) { m_controller.onSoftButtonEvent(btn, e); },
                     [this](Step btn, ButtonEvent e) { m_controller.onStepButtonEvent(btn, e); },
                     [this](Knob k, int inc) { m_controller.onErpInc(k, inc); })
  {
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

  void Ui::setLed(Midi::Led l, Midi::Color c)
  {
    for(auto &a : m_inputDevices)
      a.second->send({ 0x94, static_cast<uint8_t>(l), static_cast<uint8_t>(c) });
  }

}
