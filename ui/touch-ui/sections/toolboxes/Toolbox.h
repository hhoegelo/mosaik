#pragma once

#include <ui/Types.h>
#include <gtkmm/box.h>

namespace Ui::Touch
{
  class ToolboxesInterface;

  class Toolbox : public Gtk::Box
  {
   public:
    Toolbox(ToolboxesInterface &toolboxes, Ui::Toolbox tool, const char *title, Gtk::Widget *minimzed,
            Gtk::Widget *maximized);

   private:
  };

}
