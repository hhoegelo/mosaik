#include "CombinedGrid.h"

#include <gtkmm/box.h>
#include <gtkmm/label.h>

constexpr int knobGroupXoffset = 0;
constexpr int knobGroupYoffset = 20;
constexpr int buttonGroupXOffset = 0;
constexpr int buttonGroupYOffset = 10;

constexpr int buttonSizeX = 10;
constexpr int buttonSizeY = 20;

constexpr int knobSizeX = 10;
constexpr int knobSizeY = 20;

namespace Ui::Touch
{
  constexpr std::array c_knobPositions
      = { std::pair { Knob::Leftmost,  std::pair {  0+knobGroupXoffset,  0+knobGroupYoffset } },
          std::pair { Knob::Rightmost, std::pair { 40+knobGroupXoffset,  0+knobGroupYoffset } },
          std::pair { Knob::NorthWest, std::pair { 10+knobGroupXoffset, 10+knobGroupYoffset } },
          std::pair { Knob::NorthEast, std::pair { 30+knobGroupXoffset, 10+knobGroupYoffset } },
          std::pair { Knob::Center,    std::pair { 20+knobGroupXoffset, 30+knobGroupYoffset } },
          std::pair { Knob::SouthWest, std::pair { 10+knobGroupXoffset, 50+knobGroupYoffset } },
          std::pair { Knob::SouthEast, std::pair { 30+knobGroupXoffset, 50+knobGroupYoffset } } };

  constexpr std::array c_buttonPositions = { std::pair { SoftButton::Right_North,       std::pair {  90+buttonGroupXOffset, 20+buttonGroupYOffset } },
                                             std::pair { SoftButton::Right_NorthEast,   std::pair { 110+buttonGroupXOffset, 20+buttonGroupYOffset } },
                                             std::pair { SoftButton::Right_West,        std::pair {  80+buttonGroupXOffset, 40+buttonGroupYOffset } },
                                             std::pair { SoftButton::Right_Center,      std::pair {  90+buttonGroupXOffset, 40+buttonGroupYOffset } },
                                             std::pair { SoftButton::Right_East,        std::pair { 110+buttonGroupXOffset, 40+buttonGroupYOffset } },
                                             std::pair { SoftButton::Right_SouthWest,   std::pair {  80+buttonGroupXOffset, 60+buttonGroupYOffset } },
                                             std::pair { SoftButton::Right_South,       std::pair {  90+buttonGroupXOffset, 60+buttonGroupYOffset } },
                                             std::pair { SoftButton::Right_SouthEast,   std::pair { 110+buttonGroupXOffset, 60+buttonGroupYOffset } } };

  CombinedGrid::CombinedGrid(const char* title)
  {
    set_row_homogeneous(true);
    set_column_homogeneous(true);
    get_style_context()->add_class("knobs");
    get_style_context()->add_class("buttons");

    auto headline = Gtk::make_managed<Gtk::Label>(title);
    attach(*headline, 0, 0, 50, 10);
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
      attach(*box, c.second.first, c.second.second, knobSizeX, knobSizeY);
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
      attach(*box, c.second.first, c.second.second, buttonSizeX, buttonSizeY);
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

        attach(*box, c.second.first, c.second.second, knobSizeX, knobSizeY);
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

        attach(*box, c.second.first, c.second.second, buttonSizeX, buttonSizeY);
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
