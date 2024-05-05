#pragma once

#include "Types.h"
#include "core/api/Interface.h"
#include "tools/ReactiveVar.h"
#include "ui/midi-ui/Interface.h"
#include "tools/ReactiveVar.h"
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
  }

  class Controller
  {
   public:
    Controller(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp);

    void init(Ui::Touch::Interface &touchUi);
    void run();

    void addUi(Ui::Midi::Interface &midiUI);

    void onErpInc(Knob k, int inc);
    void onSoftButtonEvent(SoftButton b, ButtonEvent e);
    void onStepButtonEvent(Step b, ButtonEvent e);

    std::string getDisplayValue(Core::TileId tile, Core::ParameterId id);
    std::string getDisplayValue(Core::ParameterId id);

    template <typename ID> std::string getDisplayValue();

   private:
    void setLed(Led led, Color color);
    void showPattern();

    struct Mapping
    {
      std::map<Knob, std::function<void(int)>> knobIncDecReleased;
      std::map<Knob, std::function<void(int)>> knobIncDecPressed;
      std::map<Knob, std::function<void()>> knobClick;
      std::map<SoftButton, std::function<void()>> buttonPressed;
      std::map<SoftButton, std::function<void()>> buttonReleased;
    };

    Mapping createMapping(Ui::Toolbox t);
    template <Ui::Toolbox T> Mapping buildMapping();

    template <Toolbox T, typename D> std::pair<Knob, std::function<void(int)>> bindKnobUiParameterAction();
    template <Toolbox T, typename D> std::pair<SoftButton, std::function<void()>> bindButtonUiParameterAction();
    template <Toolbox T, typename D> std::pair<SoftButton, std::function<void()>> bindButtonUiInvokeAction();
    template <Toolbox T, typename D> std::pair<Knob, std::function<void(int)>> bindKnobUiInvokeAction();
    template <Toolbox T, typename D> std::pair<Knob, std::function<void()>> bindKnobUiClickAction();

    template <Toolbox T, typename D> void invokeButtonAction();
    template <Toolbox T, typename D> void invokeKnobAction(int incs);

    void processStepsGapsWizard();

    Core::Api::Interface &m_core;
    Tools::ReactiveVar<Ui::Touch::Interface *> m_touchUi { nullptr };

    std::vector<Ui::Midi::Interface *> m_midiUi;

    Tools::DeferredComputations m_computations;
    Mapping m_inputMapping;

    Tools::ReactiveVar<Step> m_currentStep { 0 };
    std::array<Color, static_cast<size_t>(Led::NUM_LEDS)> m_ledLatch {};

    std::array<bool, static_cast<size_t>(SoftButton::MAX_BUTTON_ID)> m_buttonState {};
    std::array<bool, static_cast<size_t>(Knob::NUM_KNOBS)> m_turnWhilePressed {};

    /// Wizard
    Tools::ReactiveVar<int> m_oneFitsAllStepWizard { 0 };
    Tools::ReactiveVar<int> m_wizardSteps { 0 };
    Tools::ReactiveVar<int> m_wizardGaps { 0 };
    std::chrono::system_clock::time_point m_stepWizardLastUsage = std::chrono::system_clock::time_point::min();
  };
}