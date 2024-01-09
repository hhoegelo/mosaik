#pragma once

#include <memory>
#include <sigc++/trackable.h>
#include <map>

namespace Core::Api
{
  class Interface;
}

namespace Midi
{
  class AlsaIn;
  class Monitor;
}

namespace Dsp::Api::Display
{
  class Interface;
}

namespace Ui::Midi
{
  class Ui : public sigc::trackable
  {
   public:
    Ui(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp);
    ~Ui();

   private:
    bool checkForMidiDevices();

    std::unique_ptr<::Midi::Monitor> m_monitor;
    std::map<std::string, std::unique_ptr<::Midi::AlsaIn>> m_devices;
  };
}
