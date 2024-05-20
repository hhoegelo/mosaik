#include <glibmm/main.h>
#include "Sync.h"
#include "Monitor.h"
#include "Alsa.h"
#include <core/Types.h>
#include <core/api/Interface.h>
#include <future>
#include <atomic>

namespace Midi
{
  struct Sync::Sender
  {
    Sender(Core::Api::Interface &core, const std::string &device)
        : m_device(std::make_unique<::Midi::Alsa>(device, [this](const Alsa::MidiEvent &event) {}))
    {
      m_clockSender
          = std::async(std::launch::async,
                       [this, &core]
                       {
                         while(!m_stopSender)
                         {
                           m_device->send(0xF8);
                           auto bpm = 24 * std::get<float>(core.getParameter({}, Core::ParameterId::GlobalTempo));
                           auto diff = std::chrono::nanoseconds(static_cast<uint64_t>(60 * std::nano::den / bpm));
                           std::this_thread::sleep_for(diff);
                         }
                       });
    }

    ~Sender()
    {
      m_stopSender = true;
      m_clockSender.wait();
    }

    std::unique_ptr<Alsa> m_device;
    std::atomic_bool m_stopSender { false };
    std::future<void> m_clockSender;
  };

  Sync::Sync(Core::Api::Interface &core, const std::string &inDevice, const std::string &outDevice)
      : m_core(core)
      , m_inDeviceName(inDevice)
      , m_outDeviceName(outDevice)
      , m_monitor(std::make_unique<::Midi::Monitor>())
      , m_timer(Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &Sync::checkForMidiDevices), 1))
  {
  }

  Sync::~Sync() = default;

  bool Sync::checkForMidiDevices()
  {
    m_monitor->poll(
        [this](auto &foundDevice)
        {
          if(foundDevice == m_inDeviceName)
          {
            m_inDevice = std::make_unique<::Midi::Alsa>(foundDevice,
                                                        [this](const Alsa::MidiEvent &event)
                                                        {
                                                          if(event.data()[0] == 0xF8)
                                                            onMidiClock();
                                                          else if(event.data()[0] == 0xFA)
                                                            one();
                                                        });
          }
          if(foundDevice == m_outDeviceName)
          {
            m_sender = std::make_unique<Sender>(m_core, foundDevice);
          }
        },
        [this](auto lostDevice)
        {
          if(lostDevice == m_inDeviceName)
            m_inDevice.reset();
          if(lostDevice == m_outDeviceName)
            m_sender.reset();
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