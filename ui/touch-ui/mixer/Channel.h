#pragma once
#include <gtkmm/styleproperty.h>
#include <gtkmm/box.h>
#include "core/Types.h"
#include "tools/ReactiveVar.h"

namespace Core::Api
{
  class Interface;
}

namespace Dsp::Api::Display
{
  class Interface;
}

namespace Ui
{
  class Controller;
}

namespace Ui::Touch
{
  class Channel : public Gtk::Box
  {
   public:
    Channel(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp, Ui::Controller &controller,
            Core::Address address);

    Gtk::SizeRequestMode get_request_mode_vfunc() const override;
    void get_preferred_height_vfunc(int &minimum_height, int &natural_height) const override;
    void get_preferred_width_vfunc(int &minimum_width, int &natural_width) const override;

   private:
    Tools::DeferredComputations m_computations;
    Gtk::StyleProperty<int> m_size;
    Tools::ReactiveVar<std::tuple<float, float>> m_levels {};
  };
}