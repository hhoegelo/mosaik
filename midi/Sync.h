#pragma once

#include <memory>
#include <chrono>
#include <sigc++/connection.h>

namespace Core::Api
{
  class Interface;
}

namespace Midi
{
  class Monitor;
  class Alsa;

  class Sync
  {
   public:
    Sync(Core::Api::Interface &core, const std::string &device);

   private:
    bool checkForMidiDevices();
    void one();
    void onMidiClock();

    Core::Api::Interface &m_core;
    std::string m_deviceName;
    std::unique_ptr<Monitor> m_monitor;
    sigc::connection m_timer;
    std::unique_ptr<Alsa> m_device;

    std::array<std::chrono::system_clock::time_point, 25> m_midiSyncEvents;
    size_t m_midiSyncHead = 0;
    float m_filteredSync = 0.0f;
  };

}
