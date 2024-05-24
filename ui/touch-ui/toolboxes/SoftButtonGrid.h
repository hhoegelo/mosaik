#pragma once

#include "ui/Types.h"
#include <gtkmm/grid.h>
#include "tools/ReactiveVar.h"

namespace Ui::Touch
{
  class SoftButtonGrid : public Gtk::Grid
  {
   public:
    enum class Where
    {
      Left,
      Right
    };

    explicit SoftButtonGrid(Where w);
    bool set(Ui::SoftButton btn, const char *title, Ui::Color color, std::function<std::string()> cb);

   private:
    Where m_where;
    Tools::DeferredComputations m_computations;
  };
}
