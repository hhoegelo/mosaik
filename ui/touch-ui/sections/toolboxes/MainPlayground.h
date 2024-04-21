#pragma once

#include <gtkmm/box.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  class MainPlayground : public Gtk::Box
  {
   public:
    MainPlayground(Core::Api::Interface &core);
  };
}
