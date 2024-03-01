#pragma once

#include <tools/SignalSlot.h>
#include "core/api/Interface.h"

namespace Core::Api
{
  class Interface;
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
    class Interface;

    class Controller
    {
     public:
      Controller(SharedState &sharedUiState, Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp,
                 Ui::Midi::Interface &ui);

      void kickOff();

     private:
      void showPattern();

      Core::Api::Interface &m_core;

      std::unique_ptr<Core::Api::Computation> m_pattern;
      Interface &m_ui;
    };
  }
}