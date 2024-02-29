#pragma once

#include <memory>
#include <sigc++/connection.h>
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

namespace Ui
{
  class SharedState;

  namespace Midi
  {
    class Ui
    {
     public:
      Ui(SharedState &sharedUiState, Core::Api::Interface &core);
      ~Ui();

     private:
      bool checkForMidiDevices();

      std::unique_ptr<::Midi::Monitor> m_monitor;
      sigc::connection m_timer;
      std::map<std::string, std::unique_ptr<::Midi::AlsaIn>> m_devices;
    };
  }
}
