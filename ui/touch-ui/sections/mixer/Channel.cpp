#include "Channel.h"
#include "ui/touch-ui/sections/LevelMeter.h"
#include "ui/ParameterDescriptor.h"
#include <core/api/Interface.h>
#include <dsp/api/display/Interface.h>
#include <gtkmm/label.h>

namespace Ui::Touch
{
  Channel::Channel(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Ui::Controller& controller,
                   Core::Address address)
      : Glib::ObjectBase("Channel")
      , Gtk::Box(Gtk::ORIENTATION_HORIZONTAL)
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

    auto left = new LevelMeter(
        "level left", [this] { return LevelMeter::ampToLevelMeter(std::get<0>(m_levels.get())); },
        LevelMeter::c_levelMeterDecay);

    auto gain = new LevelMeter("gain",
                               [this, &core, address]
                               {
                                 using T = Core::ParameterDescriptor<Core::ParameterId::ChannelVolume>;
                                 auto v = std::get<float>(core.getParameter(address, Core::ParameterId::ChannelVolume));
                                 return (v - T::min) / (T::max - T::min);
                               });

    auto right = new LevelMeter(
        "level right", [this] { return LevelMeter::ampToLevelMeter(std::get<1>(m_levels.get())); },
        LevelMeter::c_levelMeterDecay);

    auto levels = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
    levels->pack_start(*Gtk::manage(left), false, false);
    levels->pack_start(*Gtk::manage(gain), false, false);
    levels->pack_start(*Gtk::manage(right), false, false);
    left->set_size_request(4, -1);
    gain->set_size_request(4, -1);
    right->set_size_request(4, -1);

    set_center_widget(*levels);

    this->set_halign(Gtk::ALIGN_CENTER);

    Glib::signal_timeout().connect(
        [&dsp, address, this]
        {
          m_levels = dsp.getLevel(address);
          return true;
        },
        20);
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