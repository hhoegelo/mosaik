#pragma once

#include <gtkmm/notebook.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  class Toolboxes : public Gtk::Notebook
  {
   public:
    Toolboxes(Core::Api::Interface &core);
  };
}