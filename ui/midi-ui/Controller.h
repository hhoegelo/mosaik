#pragma once

#include <ui/Types.h>
#include <core/api/Interface.h>
#include <tools/ReactiveVar.h>
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
  namespace Touch
  {
    class Interface;
  }

  namespace Midi
  {
    class Interface;

    class Controller
    {
     public:
      Controller(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp, Ui::Touch::Interface &touchUi,
                 Ui::Midi::Interface &midiUi);

      void onErpInc(Knob k, int inc);
      void onSoftButtonEvent(SoftButton b, ButtonEvent e);
      void onStepButtonEvent(Step b, ButtonEvent e);

     private:
      void setLed(Led led, Color color);
      void showPattern();

      std::pair<Knob, std::function<void(int)>> standardBind(Knob k, Core::ParameterId p);
      std::pair<SoftButton, std::function<void()>> standardBindRelease(SoftButton k, Core::ParameterId p);
      std::pair<Knob, std::function<void(int)>> standardZoomedBind(Knob k, Core::ParameterId p);

      struct Mapping
      {
        std::map<Knob, std::function<void(int)>> knobs;
        std::map<SoftButton, std::function<void(ButtonEvent)>> buttons;
        std::map<SoftButton, std::function<void()>> buttonPresses;
        std::map<SoftButton, std::function<void()>> buttonReleases;
      };

      Mapping createMapping(Ui::Toolboxes t);
      Mapping buildTileMapping();
      Mapping buildStepMapping();
      Mapping buildGlobalMapping();
      Mapping buildWaveformMapping();
      Mapping buildPlaygroundMapping();

      Core::Api::Interface &m_core;
      Ui::Midi::Interface &m_midiUi;
      Ui::Touch::Interface &m_touchUi;

      Tools::DeferredComputations m_computations;
      Mapping m_inputMapping;

      Tools::ReactiveVar<Step> m_currentStep { 0 };
      std::array<Color, static_cast<size_t>(Led::NUM_LEDS)> m_ledLatch {};
    };
  }
}