#pragma once

#include "ui/Types.h"
#include <gtkmm/grid.h>
#include "tools/ReactiveVar.h"

namespace Ui::Touch
{
  class CombinedGrid : public Gtk::Grid
  {
   public:
    CombinedGrid(const char *title);
    void set(Ui::Knob btn, const char *title, Ui::Color color, std::function<std::string()> cb);
    void set(Ui::SoftButton btn, const char *title, Ui::Color color, std::function<std::string()> cb);

    Gtk::SizeRequestMode get_request_mode_vfunc() const override;
    void get_preferred_height_vfunc(int &minimum_height, int &natural_height) const override;
    void get_preferred_width_vfunc(int &minimum_width, int &natural_width) const override;

   private:
    Tools::DeferredComputations m_computations;
  };

}
