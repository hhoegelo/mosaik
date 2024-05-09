#include <glibmm/main.h>
#include "Sync.h"
#include "Monitor.h"
#include "Alsa.h"
#include <core/Types.h>
#include <core/api/Interface.h>

namespace Midi
{
  Sync::Sync(Core::Api::Interface &core, const std::string &device)
      : m_core(core)
      , m_deviceName(device)
      , m_monitor(std::make_unique<::Midi::Monitor>())
      , m_timer(Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &Sync::checkForMidiDevices), 1))
  {
  }

  bool Sync::checkForMidiDevices()
  {
    m_monitor->poll(
        [this](auto &foundDevice)
        {
          if(foundDevice == m_deviceName)
            m_device = std::make_unique<::Midi::Alsa>(foundDevice,
                                                      [this](const Alsa::MidiEvent &event)
                                                      {
                                                        if(event.data()[0] == 0xF8)
                                                          onMidiClock();
                                                        else if(event.data()[0] == 0xFA)
                                                          one();
                                                      });
        },
        [this](auto lostDevice)
        {
          if(lostDevice == m_deviceName)
            m_device.reset();
        });
    return true;
  }

  void Sync::onMidiClock()
  {
    auto now = std::chrono::system_clock::now();
    m_midiSyncEvents[m_midiSyncHead % m_midiSyncEvents.size()] = now;
    m_midiSyncHead++;

    if(m_midiSyncHead >= m_midiSyncEvents.size())
    {
      auto diff = now - m_midiSyncEvents[(m_midiSyncHead - m_midiSyncEvents.size()) % m_midiSyncEvents.size()];
      auto bpm = 60.f * std::nano::den / diff.count();

      if(m_filteredSync == 0.0f)
        m_filteredSync = bpm;

      float coef = 0.95;
      m_filteredSync = coef * m_filteredSync + (1.f - coef) * bpm;
      m_core.setParameter({}, Core::ParameterId::GlobalTempo, m_filteredSync);
    }
  }

  void Sync::one()
  {
    m_core.addTap();
  }

}