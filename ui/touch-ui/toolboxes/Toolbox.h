#pragma once

#include "ui/Types.h"
#include <gtkmm/box.h>
#include "tools/ReactiveVar.h"

namespace Ui::Touch
{
  class ToolboxesInterface;

  class Toolbox : public Gtk::Box
  {
   public:
    Toolbox(ToolboxesInterface &toolboxes, Ui::Toolbox tool, Gtk::Widget *maximized);

   protected:
    Tools::DeferredComputations m_computations;
  };

}
