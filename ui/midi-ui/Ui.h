#pragma once

namespace Core
{
  namespace Api
  {
    class Interface;
  }
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
    };
  }
}