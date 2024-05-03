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

      struct Mapping
      {
        std::map<Knob, std::function<void(int)>> knobIncDecReleased;
        std::map<Knob, std::function<void(int)>> knobIncDecPressed;
        std::map<SoftButton, std::function<void()>> buttonPressed;
        std::map<SoftButton, std::function<void()>> buttonReleased;
      };

      Mapping createMapping(Ui::Toolbox t);
      template <Ui::Toolbox T> Mapping buildMapping();

      template <Toolbox T, typename D> std::pair<Knob, std::function<void(int)>> bindKnobUiParameterAction();
      template <Toolbox T, typename D> std::pair<SoftButton, std::function<void()>> bindButtonUiParameterAction();
      template <Toolbox T, typename D> std::pair<SoftButton, std::function<void()>> bindButtonUiInvokeAction();
      template <Toolbox T, typename D> std::pair<Knob, std::function<void(int)>> bindKnobUiInvokeAction();

      template <Toolbox T, typename D> void invokeButtonAction();
      template <Toolbox T, typename D> void invokeKnobAction(int incs);

      Core::Api::Interface &m_core;
      Ui::Midi::Interface &m_midiUi;
      Ui::Touch::Interface &m_touchUi;

      Tools::DeferredComputations m_computations;
      Mapping m_inputMapping;

      Tools::ReactiveVar<Step> m_currentStep { 0 };
      std::array<Color, static_cast<size_t>(Led::NUM_LEDS)> m_ledLatch {};

      std::array<bool, static_cast<size_t>(SoftButton::MAX_BUTTON_ID)> m_buttonState {};

      int m_stepWizard = 0;
      std::chrono::system_clock::time_point m_stepWizardLastUsage = std::chrono::system_clock::time_point::min();
    };
  }
}