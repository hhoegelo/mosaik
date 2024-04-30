#pragma once

#include <gtkmm/box.h>
#include "core/Types.h"

namespace Ui::Touch
{
  class MinimizedToolbox : public Gtk::Box
  {
   public:
    MinimizedToolbox(std::initializer_list<Core::ParameterId> parameters);
  };
}