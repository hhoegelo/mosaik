#pragma once

#include <gtkmm/box.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  class Playground : public Gtk::Box
  {
   public:
    Playground(Core::Api::Interface &core);
  };
}
