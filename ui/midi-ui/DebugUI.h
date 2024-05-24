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

  class Controller;

  namespace Midi
  {
    class DebugUI : public Gtk::Window, public Interface
    {
     public:
      explicit DebugUI(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp, ::Ui::Touch::Interface &touchUi,
                       ::Ui::Controller &controller);
      ~DebugUI() override;

     private:
      void build();
      void setLed(Knob l, Color c, Brightness bright) override;
      void setLed(SoftButton s, Color c, Brightness bright) override;
      void setLed(Step s, Color c, Brightness bright) override;
      void setLed(SoftButton s, uint8_t r, uint8_t g, uint8_t b) override;

      void setColor(const std::string &widgetName, Color c);
      void setColor(const Gtk::Widget *widget, Color c);
      void setColor(const Widget *widget, uint8_t r, uint8_t g, uint8_t b);

      Widget *buildStep(Step step);
      Widget *buildKnob(Knob knob);
      Widget *buildButton(SoftButton button);

      Core::Api::Interface &m_core;
      ::Ui::Controller &m_ctrl;

      const Widget *findChild(const std::string &name);
    };
  }
}