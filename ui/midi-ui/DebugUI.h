#pragma once

#include "Interface.h"
#include <gtkmm-3.0/gtkmm.h>

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
    class Controller;

    class DebugUI : public Gtk::Window, public Interface
    {
     public:
      explicit DebugUI(SharedState &sharedUiState, Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp);
      ~DebugUI() override;

      void build();

     private:
      void setSoftButtonColor(SoftButton button, Color c) override;
      void setStepButtonColor(Step step, Color c) override;
      void onSoftButtonEvent(std::function<void(SoftButton, ButtonEvent)> cb) override;
      void onStepButtonEvent(std::function<void(Step step, ButtonEvent)> cb) override;
      void onKnobEvent(std::function<void(Knob knob, int increments)> cb) override;
      void highlightCurrentStep(Step oldStep, Step newStep) override;

      void setColor(const std::string &widgetName, Color c);

      Widget *buildStep(Step step);
      Widget *buildKnob(Knob knob);
      Widget *buildButton(SoftButton button);

      Core::Api::Interface &m_core;
      std::unique_ptr<Controller> m_ctrl;

      std::function<void(SoftButton, ButtonEvent)> m_onSoftButtonEvent;
      std::function<void(Step, ButtonEvent)> m_onStepButtonEvent;
      std::function<void(Knob, int)> m_onKnobEvent;
      const Widget *findChild(const std::string &name);
    };
  }
}