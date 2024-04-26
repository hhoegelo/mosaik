#include "Toolbox.h"
#include <ui/touch-ui/Interface.h>
#include <gtkmm/label.h>
#include <gtkmm/eventbox.h>

namespace Ui::Touch
{

  Toolbox::Toolbox(ToolboxesInterface &toolboxes, Ui::Toolbox tool, const char *title, Gtk::Widget *minimzed,
                   Gtk::Widget *maximized)
  {
    set_orientation(Gtk::ORIENTATION_VERTICAL);

    get_style_context()->add_class("toolbox");
    auto headline = Gtk::manage(new Gtk::Label(title));
    headline->get_style_context()->add_class("header");

    auto eventBox = Gtk::manage(new Gtk::EventBox());
    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    eventBox->add(*box);

    eventBox->signal_button_press_event().connect(
        [=, &toolboxes](GdkEventButton *)
        {
          toolboxes.selectToolbox(tool);
          return true;
        });

    box->add(*headline);
    box->add(*minimzed);

    add(*eventBox);
    add(*maximized);

    auto setChildVisability = [=, &toolboxes]
    {
      auto sel = toolboxes.getSelectedToolbox() == tool;
      minimzed->set_visible(!sel);
      maximized->set_visible(sel);
    };

    m_computations.add(setChildVisability);
    signal_realize().connect(setChildVisability);
  }
}
