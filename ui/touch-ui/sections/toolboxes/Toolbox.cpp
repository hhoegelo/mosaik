#include "Toolbox.h"
#include <ui/touch-ui/Interface.h>
#include <gtkmm/label.h>
#include <gtkmm/eventbox.h>

namespace Ui::Touch
{

  Toolbox::Toolbox(ToolboxesInterface &toolboxes, Ui::Toolbox tool, Gtk::Widget *minimzed, Gtk::Widget *maximized)
  {
    set_orientation(Gtk::ORIENTATION_VERTICAL);

    get_style_context()->add_class("toolbox");

    auto eventBox = Gtk::manage(new Gtk::EventBox());
    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    eventBox->add(*box);

    eventBox->signal_button_press_event().connect(
        [=, &toolboxes](GdkEventButton *)
        {
          toolboxes.selectToolbox(tool);
          return true;
        });

    box->pack_start(*minimzed, Gtk::PACK_SHRINK);
    pack_start(*eventBox, Gtk::PACK_SHRINK);
    pack_start(*maximized, Gtk::PACK_EXPAND_WIDGET);

    auto checkChildVisibility = [=, &toolboxes]
    {
      auto sel = toolboxes.getSelectedToolbox() == tool;
      minimzed->set_visible(!sel);
      maximized->set_visible(sel);
    };

    m_computations.add(checkChildVisibility);
    signal_realize().connect(checkChildVisibility);
  }
}
