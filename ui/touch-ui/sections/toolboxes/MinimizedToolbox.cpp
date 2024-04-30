#include "MinimizedToolbox.h"

#include <gtkmm/label.h>

namespace Ui::Touch
{

  Ui::Touch::MinimizedToolbox::MinimizedToolbox(std::initializer_list<Core::ParameterId> parameters)
  {
    add(*Gtk::manage(new Gtk::Label("min")));
  }

}