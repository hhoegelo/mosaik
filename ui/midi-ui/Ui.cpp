#include <memory>
#include <utility>
#include "Ui.h"
#include <midi/Monitor.h>
#include <midi/Alsa.h>
#include <glibmm.h>

namespace Ui::Midi
{

  Ui::Ui(std::string midiDevice, Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp,
         ::Ui::Touch::Interface &touchUi, ::Ui::Controller &controller)
      : m_midiDevice(std::move(midiDevice))
      , m_controller(controller)
      , m_monitor(std::make_unique<::Midi::Monitor>())
      , m_timer(Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &Ui::checkForMidiDevices), 1))
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
    for(auto &a : m_inputDevices)
      a.second->send({ 0x97, 0, 0 });

    m_timer.disconnect();
    m_ledUpdater.disconnect();
  }

  void Ui::setLed(Knob k, Color c)
  {
    uint8_t firstLed = static_cast<uint8_t>(Led::FirstCenterKnob);
    uint8_t numKnob = static_cast<uint8_t>(k);
    uint8_t numLedsPerKnob = 4;
    uint8_t led = firstLed + numKnob * numLedsPerKnob;

    for(auto &a : m_inputDevices)
      for(uint8_t b = 0; b < numLedsPerKnob; b++)
        a.second->send({ 0x94, static_cast<uint8_t>(led + b), static_cast<uint8_t>(c) });

    scheduleLedUpdate();
  }

  void Ui::setLed(SoftButton k, Color c)
  {
    uint8_t led = 0;

    if((k >= SoftButton::FirstLeftButton && k <= SoftButton::LastLeftButton))
    {
      uint8_t offset = static_cast<uint8_t>(k) - static_cast<uint8_t>(SoftButton::FirstLeftButton);
      led = static_cast<uint8_t>(Led::FirstLeftButton) + offset;
    }

    if((k >= SoftButton::FirstRightButton && k <= SoftButton::LastRightButton))
    {
      uint8_t offset = static_cast<uint8_t>(k) - static_cast<uint8_t>(SoftButton::FirstRightButton);
      led = static_cast<uint8_t>(Led::FirstRightButton) + offset;
    }

    for(auto &a : m_inputDevices)
      a.second->send({ 0x94, led, static_cast<uint8_t>(c) });

    scheduleLedUpdate();
  }

  void Ui::setLed(Step k, Color c)
  {
    for(auto &a : m_inputDevices)
      a.second->send({ 0x94, k, static_cast<uint8_t>(c) });

    scheduleLedUpdate();
  }

  void Ui::scheduleLedUpdate()
  {
    if(!m_ledUpdater.connected())
      m_ledUpdater = Glib::signal_timeout().connect(
          [this]
          {
            for(auto &a : m_inputDevices)
              a.second->send({ 0x95, 0, 0 });
            return false;
          },
          1);
  }

}
