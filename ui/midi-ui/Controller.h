#pragma once

#include <core/api/Interface.h>
#include <ui/SharedState.h>
#include <ui/midi-ui/Interface.h>
#include <tools/ReactiveVar.h>
#include <map>

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
      void onSoftButtonEvent(SoftButton b, ButtonEvent e);
      void onStepButtonEvent(Step b, ButtonEvent e);

     private:
      void showPattern();

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

      Tools::DeferredComputations m_computations;
      Mapping m_inputMapping;
      Controller::Mapping buildWaveformMapping();
    };
  }
}