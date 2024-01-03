#pragma once

#include <memory>

namespace Core
{
  namespace Api
  {
    class Interface;
  }
}

namespace Midi
{
  class AlsaIn;
  class Monitor;
}

namespace Dsp
{
  namespace Api
  {
    namespace Display
    {
      class Interface;
    }
  }
}

namespace Ui
{
  namespace Midi
  {
    class Ui
    {
     public:
      Ui(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp);
      ~Ui();

     private:
      std::unique_ptr<::Midi::Monitor> m_monitor;
    };
  }
}