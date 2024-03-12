#pragma once

#include <core/api/Interface.h>
#include <tools/SignalSlot.h>
#include <ui/SharedState.h>
#include "Interface.h"

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

      void onErpInc(Knob k, int inc);
      void onButtonEvent(SoftButton b, ButtonEvent e);

     private:
      void showPattern(Core::Api::Computation *c);

      struct Mapping
      {
        std::map<Knob, std::function<void(int)>> knobs;
        std::map<SoftButton, std::function<void(ButtonEvent)>> buttons;
      };

      Mapping createMapping(Ui::SharedState::Toolboxes t);
      Mapping buildTileMapping();
      Mapping buildGlobalMapping();

      SharedState &m_sharedUiState;
      Core::Api::Interface &m_core;
      Interface &m_ui;

      Core::Api::Computations m_computations;
      Tools::Signals::Connection m_selectedTool;

      Mapping m_inputMapping;
      Controller::Mapping buildWaveformMapping();
    };
  }
}