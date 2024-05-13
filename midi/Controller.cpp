
#include <glibmm/main.h>
#include "Controller.h"
#include "Alsa.h"
#include "Monitor.h"
#include <core/api/Interface.h>

namespace Midi
{
  Controller::Controller(const std::string &device, Core::Api::Interface &core)
      : m_core(core)
      , m_deviceName(device)
      , m_monitor(std::make_unique<::Midi::Monitor>())
      , m_timer(Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &Controller::checkForMidiDevices), 1))
  {
  }

  Controller::~Controller() = default;

  bool Controller::checkForMidiDevices()
  {
    m_monitor->poll(
        [this](auto &foundDevice)
        {
          if(foundDevice == m_deviceName)
            m_device = std::make_unique<::Midi::Alsa>(foundDevice, [this](const auto &event) { this->doMidi(event); });
        },
        [this](auto lostDevice)
        {
          if(lostDevice == m_deviceName)
            m_device.reset();
        });
    return true;
  }

  void Controller::doMidi(const Controller::MidiEvent &e)
  {
    if(e[0] == 0xB0)
    {
      auto c = e[1];
      auto v = e[2];

      if(c < NUM_CHANNELS)
        m_core.setParameter({ c, {} }, Core::ParameterId::ChannelVolume, v / 127.0f);
    }
  }

}
