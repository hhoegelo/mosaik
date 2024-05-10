#include "Channel.h"
#include <core/api/Interface.h>
#include <gtkmm/label.h>

namespace Ui::Touch
{
  Channel::Channel(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Ui::Controller& controller,
                   Core::Address address)
      : Glib::ObjectBase("Channel")
      , Gtk::Grid()
      , m_size(*this, "size", 50)
  {
    get_style_context()->add_class("channel");

    m_computations.add(
        [this, &core, address]
        {
          if(core.getSelectedTile().channel == address.channel)
            get_style_context()->add_class("selected");
          else
            get_style_context()->remove_class("selected");
        });

    add(*Gtk::manage(new Gtk::Label("foo")));
  }

  Gtk::SizeRequestMode Channel::get_request_mode_vfunc() const
  {
    return Gtk::SIZE_REQUEST_CONSTANT_SIZE;
  }

  void Channel::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
  {
    int m = m_size.get_value();
    minimum_height = natural_height = m / 2;
  }

  void Channel::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
  {
    int m = m_size.get_value();
    minimum_width = natural_width = m;
  }
}