#pragma once

#include "ui/Types.h"
#include <gtkmm/grid.h>
#include "tools/ReactiveVar.h"

namespace Ui::Touch
{
  class KnobGrid : public Gtk::Grid
  {
   public:
    KnobGrid();
    void set(Ui::Knob btn, const char *title, std::function<std::string()> cb);

   private:
    Tools::DeferredComputations m_computations;
  };

}
