#pragma once

#include <gtkmm-3.0/gtkmm.h>

namespace Core
{
  namespace Api
  {
    class Interface;
  }
}

namespace Dsp
{
  namespace Api
  {
    namespace Display
    {
      class Interface;
    }
  }
}

namespace Ui
{
  namespace Touch
  {
    class Window : public Gtk::Window
    {
     public:
      Window(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp);
    };
  }
}