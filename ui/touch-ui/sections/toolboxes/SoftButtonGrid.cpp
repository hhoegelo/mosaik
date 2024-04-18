#include "SoftButtonGrid.h"

#include <gtkmm/box.h>
#include <gtkmm/label.h>

namespace Ui::Touch
{
  constexpr std::array c_leftPositions = {
    std::pair { SoftButton::Left_NorthWest, std::pair { 0, 0 } },
    std::pair { SoftButton::Left_North, std::pair { 1, 0 } },
    std::pair { SoftButton::Left_West, std::pair { 0, 1 } },
    std::pair { SoftButton::Left_Center, std::pair { 1, 1 } },
    std::pair { SoftButton::Left_East, std::pair { 2, 1 } },
    std::pair { SoftButton::Left_SouthWest, std::pair { 0, 2 } },
    std::pair { SoftButton::Left_South, std::pair { 1, 2 } },
    std::pair { SoftButton::Left_SouthEast, std::pair { 2, 2 } },
  };

  constexpr std::array c_rightPositions = {
    std::pair { SoftButton::Right_North, std::pair { 1, 0 } },
    std::pair { SoftButton::Right_NorthEast, std::pair { 2, 0 } },
    std::pair { SoftButton::Right_West, std::pair { 0, 1 } },
    std::pair { SoftButton::Right_Center, std::pair { 1, 1 } },
    std::pair { SoftButton::Right_East, std::pair { 2, 1 } },
    std::pair { SoftButton::Right_SouthWest, std::pair { 0, 2 } },
    std::pair { SoftButton::Right_South, std::pair { 1, 2 } },
    std::pair { SoftButton::Right_SouthEast, std::pair { 2, 2 } },
  };

  SoftButtonGrid::SoftButtonGrid(Where w)
      : m_where(w)
  {
    set_row_homogeneous(true);
    set_column_homogeneous(true);
    get_style_context()->add_class("buttons");

    for(auto c : w == Where::Left ? c_leftPositions : c_rightPositions)
    {
      auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
      box->get_style_context()->add_class("button");
      box->add(*Gtk::manage(new Gtk::Label("---")));
      auto level = Gtk::manage(new Gtk::Label());
      box->add(*level);
      level->set_label("---");
      attach(*box, c.second.first, c.second.second, 1, 1);
    }
  }

  void SoftButtonGrid::set(Ui::SoftButton btn, const char *title, std::function<std::string()> cb)
  {
    auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    box->get_style_context()->add_class("button");
    box->add(*Gtk::manage(new Gtk::Label(title)));
    auto level = Gtk::manage(new Gtk::Label());
    box->add(*level);
    m_computations.add([level, cb] { level->set_label(cb()); });

    for(auto c : m_where == Where::Left ? c_leftPositions : c_rightPositions)
      if(c.first == btn)
      {
        if(auto child = get_child_at(c.second.first, c.second.second))
          remove(*child);

        attach(*box, c.second.first, c.second.second, 1, 1);
        break;
      }
  }

}
