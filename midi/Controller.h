
#pragma once

#include <string>
#include <memory>
#include <sigc++/connection.h>

namespace Core::Api
{
  class Interface;
}

namespace Midi
{
  class Monitor;
  class Alsa;

  class Controller
  {
   public:
    Controller(const std::string &device, Core::Api::Interface &core);
    ~Controller();

   private:
    using MidiEvent = std::array<uint8_t, 3>;
    void doMidi(const MidiEvent &e);

    std::string m_deviceName;
    Core::Api::Interface &m_core;
    std::unique_ptr<Monitor> m_monitor;
    sigc::connection m_timer;
    std::unique_ptr<Alsa> m_device;
    bool checkForMidiDevices();
  };

}  // Midi
