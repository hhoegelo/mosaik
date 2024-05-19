#pragma once

#include "Types.h"
#include "core/api/Interface.h"
#include "tools/ReactiveVar.h"
#include "ui/midi-ui/Interface.h"
#include "tools/ReactiveVar.h"
#include "ToolboxDefinition.h"
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

    std::string getDisplayValue(Core::Address address, Core::ParameterId id) const;
    std::string getDisplayValue(Core::ParameterId id) const;

    template <typename Any> std::string getDisplayValue(const Any &) const
    {
      return {};
    }

    std::string getDisplayValue(Ui::ToolboxDefinition<Toolbox::Waveform>::Zoom) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::Waveform>::Scroll) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::Waveform>::HitPoint) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::Steps>::Gaps) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::Steps>::Steps) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::Steps>::Rotate) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::Steps>::Invert) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::Steps>::Mirror) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::Mute>::SaveArmed) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::Mute>::SaveUnarmed) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::ColorAdjust>::Led_R) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::ColorAdjust>::Led_G) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::ColorAdjust>::Led_B) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::ColorAdjust>::Screen_R) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::ColorAdjust>::Screen_G) const;
    std::string getDisplayValue(ToolboxDefinition<Toolbox::ColorAdjust>::Screen_B) const;

   private:
    void setLed(Step step, Color color);
    void setLed(SoftButton button, Color color);
    void setLed(Knob knob, Color color);

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
    template <Ui::Toolbox T> Mapping buildMapping(Ui::Toolbox t);
    template <Ui::Toolbox T> Mapping buildMapping();

    template <typename D> std::pair<Knob, std::function<void(int)>> bindKnobUiParameterAction(float factor = 1.0f);
    template <typename D> std::pair<SoftButton, std::function<void()>> bindButtonUiParameterAction();
    template <typename D> std::pair<SoftButton, std::function<void()>> bindButtonUiInvokeAction();
    template <typename D> std::pair<Knob, std::function<void(int)>> bindKnobUiInvokeAction(float factor = 1.0f);
    template <typename D> std::pair<Knob, std::function<void()>> bindKnobUiClickAction();
    template <typename D> std::pair<Knob, std::function<void()>> bindKnobUiDefaultClickAction();

    template <typename D> void invokeButtonAction();
    template <typename D> void invokeKnobAction(int incs);
    template <typename D> void invokeKnobClickAction();

    void processStepsGapsWizard();

    Core::Api::Interface &m_core;
    Tools::ReactiveVar<Ui::Touch::Interface *> m_touchUi { nullptr };

    std::vector<Ui::Midi::Interface *> m_midiUi;

    Tools::DeferredComputations m_computations;
    Mapping m_inputMapping;

    Tools::ReactiveVar<Step> m_currentStep { 0 };
    std::array<Color, static_cast<size_t>(NUM_STEPS)> m_stepsLedLatch {};
    std::array<Color, static_cast<size_t>(SoftButton::LastButton) + 1> m_softButtonLedLatch {};
    std::array<Color, static_cast<size_t>(Knob::LastKnob) + 1> m_knobsLedLatch {};

    std::array<bool, static_cast<size_t>(SoftButton::LastButton) + 1> m_buttonState {};
    std::array<bool, static_cast<size_t>(Knob::LastKnob) + 1> m_turnWhilePressed {};

    // Wizard
    Tools::ReactiveVar<int> m_oneFitsAllStepWizard { 0 };
    Tools::ReactiveVar<int> m_wizardSteps { 0 };
    Tools::ReactiveVar<int> m_wizardGaps { 0 };
    Tools::ReactiveVar<int> m_wizardRotation { 0 };
    Tools::ReactiveVar<bool> m_wizardInvert { false };
    Tools::ReactiveVar<bool> m_wizardMirror { false };

    // Mute
    using MuteStatePerChannel = std::array<bool, NUM_TILES_PER_CHANNEL>;
    using MuteState = std::array<MuteStatePerChannel, NUM_CHANNELS>;
    MuteState m_savedMute {};
    Tools::ReactiveVar<bool> m_saveArmed { false };
    std::array<Tools::ReactiveVar<MuteState>, 6> m_savedMutes;
    void handleMuteSlot(int i);

    // Color Adjustment
    std::unique_ptr<Tools::DeferredComputations> m_colorAdjustmentComputations;
    Tools::ReactiveVar<int> m_led_R;
    Tools::ReactiveVar<int> m_led_G;
    Tools::ReactiveVar<int> m_led_B;

    Tools::ReactiveVar<int> m_screen_R;
    Tools::ReactiveVar<int> m_screen_G;
    Tools::ReactiveVar<int> m_screen_B;
  };

}