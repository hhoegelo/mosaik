#include "CombinedGrid.h"

#include <gtkmm/box.h>
#include <gtkmm/label.h>

namespace Ui::Touch
{
  constexpr std::array c_knobPositions
      = { std::pair { Knob::Leftmost, std::pair { 0, 0 } },  std::pair { Knob::Rightmost, std::pair { 8, 0 } },
          std::pair { Knob::NorthWest, std::pair { 2, 1 } }, std::pair { Knob::NorthEast, std::pair { 6, 1 } },
          std::pair { Knob::Center, std::pair { 4, 2 } },    std::pair { Knob::SouthWest, std::pair { 2, 3 } },
          std::pair { Knob::SouthEast, std::pair { 6, 3 } } };

  constexpr std::array c_buttonPositions = { std::pair { SoftButton::Right_North, std::pair { 9, 4 } },
                                             std::pair { SoftButton::Right_NorthEast, std::pair { 11, 4 } },
                                             std::pair { SoftButton::Right_West, std::pair { 7, 6 } },
                                             std::pair { SoftButton::Right_Center, std::pair { 9, 6 } },
                                             std::pair { SoftButton::Right_East, std::pair { 11, 6 } },
                                             std::pair { SoftButton::Right_SouthWest, std::pair { 7, 8 } },
                                             std::pair { SoftButton::Right_South, std::pair { 9, 8 } },
                                             std::pair { SoftButton::Right_SouthEast, std::pair { 11, 8 } } };

  CombinedGrid::CombinedGrid(const char* title)
  {
    set_row_homogeneous(true);
    set_column_homogeneous(true);
    get_style_context()->add_class("knobs");
    get_style_context()->add_class("buttons");

    auto headline = Gtk::make_managed<Gtk::Label>(title);
    attach(*headline, 0, 6, 6, 4);
    headline->get_style_context()->add_class("headline");

    for(auto c : c_knobPositions)
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

    for(auto c : c_buttonPositions)
    {
      auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
      box->get_style_context()->add_class("button");
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

  void CombinedGrid::set(Ui::Knob btn, const char* title, Ui::Color color, std::function<std::string()> cb)
  {
    auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    box->get_style_context()->add_class("knob");
    box->get_style_context()->add_class(getColorName(color));
    auto label = new Gtk::Label(title);
    box->add(*Gtk::manage(label));

    auto level = Gtk::manage(new Gtk::Label());
    box->add(*level);
    m_computations.add([level, cb] { level->set_label(cb()); });

    for(auto c : c_knobPositions)
      if(c.first == btn)
      {
        if(auto child = get_child_at(c.second.first, c.second.second))
          remove(*child);

        attach(*box, c.second.first, c.second.second, 2, 2);
        break;
      }
  }

  void CombinedGrid::set(Ui::SoftButton btn, const char* title, Ui::Color color, std::function<std::string()> cb)
  {
    auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    box->get_style_context()->add_class("button");
    box->get_style_context()->add_class(getColorName(color));
    auto label = new Gtk::Label(title);
    box->add(*Gtk::manage(label));

    auto level = Gtk::manage(new Gtk::Label());
    box->add(*level);
    m_computations.add([level, cb] { level->set_label(cb()); });

    for(auto c : c_buttonPositions)
      if(c.first == btn)
      {
        if(auto child = get_child_at(c.second.first, c.second.second))
          remove(*child);

        attach(*box, c.second.first, c.second.second, 2, 2);
        break;
      }
  }

  Gtk::SizeRequestMode CombinedGrid::get_request_mode_vfunc() const
  {
    return Gtk::SIZE_REQUEST_CONSTANT_SIZE;
  }

  void CombinedGrid::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
  {
    minimum_height = natural_height = 200;
  }

  void CombinedGrid::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
  {
    minimum_width = natural_width = 200;
  }

}
