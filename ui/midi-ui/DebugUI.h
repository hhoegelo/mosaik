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
  namespace Touch
  {
    class Interface;
  }

  namespace Midi
  {
    class Controller;

    class DebugUI : public Gtk::Window, public Interface
    {
     public:
      explicit DebugUI(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp, ::Ui::Touch::Interface &touchUi);
      ~DebugUI() override;

     private:
      void build();
      void setLed(Led l, Color c) override;
      void setColor(const std::string &widgetName, Color c);

      Widget *buildStep(Step step);
      Widget *buildKnob(Knob knob);
      Widget *buildButton(SoftButton button);

      Core::Api::Interface &m_core;
      std::unique_ptr<Controller> m_ctrl;

      const Widget *findChild(const std::string &name);
    };
  }
}