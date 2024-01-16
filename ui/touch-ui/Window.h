#pragma once

#include <gtkmm-3.0/gtkmm.h>

namespace Core::Api
{
  class Interface;
}

namespace Dsp::Api::Display
{
  class Interface;
}

namespace Ui::Touch
{
  class Window : public Gtk::Window
  {
   public:
    Window(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp);
  };
}
