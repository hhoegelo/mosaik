#pragma once

#include <gtkmm/box.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{

  class GlobalTools : public Gtk::Box
  {
   public:
    explicit GlobalTools(Core::Api::Interface &core);
  };

}