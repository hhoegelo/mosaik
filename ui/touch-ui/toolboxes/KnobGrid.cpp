#include "KnobGrid.h"

#include <gtkmm/box.h>
#include <gtkmm/label.h>

namespace Ui::Touch
{
  constexpr std::array c_positions
      = { std::pair { Knob::Leftmost, std::pair { 0, 0 } },  std::pair { Knob::Rightmost, std::pair { 8, 0 } },
          std::pair { Knob::NorthWest, std::pair { 2, 1 } }, std::pair { Knob::NorthEast, std::pair { 6, 1 } },
          std::pair { Knob::Center, std::pair { 4, 2 } },    std::pair { Knob::SouthWest, std::pair { 2, 3 } },
          std::pair { Knob::SouthEast, std::pair { 6, 3 } } };

  KnobGrid::KnobGrid()
  {
    set_row_homogeneous(true);
    set_column_homogeneous(true);
    get_style_context()->add_class("knobs");

    for(auto c : c_positions)
    {
      auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
      box->get_style_context()->add_class("knob");
      auto label = Gtk::manage(new Gtk::Label(""));
      box->add(*label);
      auto level = Gtk::manage(new Gtk::Label());
      box->add(*level);
      level->set_label("");

      label->get_style_context()->add_class("label");
      level->get_style_context()->add_class("level");
      attach(*box, c.second.first, c.second.second, 2, 2);
    }
  }

  void KnobGrid::set(Ui::Knob btn, const char *title, Ui::Color color, std::function<std::string()> cb)
  {
    auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    box->get_style_context()->add_class("knob");
    box->get_style_context()->add_class(getColorName(color));
    auto label = new Gtk::Label(title);
    box->add(*Gtk::manage(label));

    auto level = Gtk::manage(new Gtk::Label());
    box->add(*level);
    m_computations.add([level, cb] { level->set_label(cb()); });

    for(auto c : c_positions)
      if(c.first == btn)
      {
        if(auto child = get_child_at(c.second.first, c.second.second))
          remove(*child);

        attach(*box, c.second.first, c.second.second, 2, 2);
        break;
      }
  }

}
