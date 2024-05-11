#include "Tile.h"
#include <core/api/Interface.h>
#include <core/ParameterDescriptor.h>
#include <dsp/api/display/Interface.h>
#include <ui/Controller.h>

#include "WaveformThumb.h"
#include "ui/touch-ui/sections/LevelMeter.h"

#include <gtkmm/label.h>

namespace Ui::Touch
{

  Tile::Tile(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Ui::Controller& controller,
             Core::Address address)
      : Glib::ObjectBase("Tile")
      , Gtk::Grid()
      , m_size(*this, "size", 50)
  {
    get_style_context()->add_class("tile");
    set_column_homogeneous(true);
    set_row_homogeneous(true);

    auto steps = addSteps();
    auto sampleName = addSampleName();
    auto waveform = addWaveform(core, dsp, address);
    auto seconds = addDurationLabel();

    attach(*Gtk::manage(new LevelMeter(
               "level left", [this] { return LevelMeter::ampToLevelMeter(std::get<0>(m_levels.get())); },
               LevelMeter::c_levelMeterDecay)),
           13, 4, 1, 8);

    attach(*Gtk::manage(new LevelMeter("gain",
                                       [this, &core, address]
                                       {
                                         using T = Core::ParameterDescriptor<Core::ParameterId::Gain>;
                                         auto v = std::get<float>(core.getParameter(address, Core::ParameterId::Gain));
                                         return (v - T::min) / (T::max - T::min);
                                       })),
           14, 4, 1, 8);

    attach(*Gtk::manage(new LevelMeter(
               "level right", [this] { return LevelMeter::ampToLevelMeter(std::get<1>(m_levels.get())); },
               LevelMeter::c_levelMeterDecay)),
           15, 4, 1, 8);

    m_computations.add(
        [this, &core, address]()
        {
          auto muted = std::get<bool>(core.getParameter(address, Core::ParameterId::Mute));
          if(muted)
            get_style_context()->add_class("muted");
          else
            get_style_context()->remove_class("muted");
        });

    m_computations.add([&controller, address, sampleName]()
                       { sampleName->set_label(controller.getDisplayValue(address, Core::ParameterId::SampleFile)); });

    m_computations.add(
        [this, &core, &dsp, address, seconds]()
        {
          auto file = std::get<Core::Path>(core.getParameter(address, Core::ParameterId::SampleFile));
          auto ms = dsp.getDuration(file).count();
          seconds->set_label(formatTime(ms));
        });

    m_computations.add(
        [&core, address, this]()
        {
          if(std::get<bool>(core.getParameter(address, Core::ParameterId::Selected)))
            get_style_context()->add_class("selected");
          else
            get_style_context()->remove_class("selected");
        });

    m_computations.add(
        [&core, address, waveform]()
        {
          auto _1 = core.getParameter(address, Core::ParameterId::SampleFile);
          auto _2 = core.getSamples(address);
          waveform->queue_draw();
        });

    m_computations.add(
        [&core, address, this, steps]()
        {
          auto pattern = std::get<Core::Pattern>(core.getParameter(address, Core::ParameterId::Pattern));

          for(size_t i = 0; i < 64; i++)
          {
            if(pattern[i])
              steps[i]->get_style_context()->add_class("selected");
            else
              steps[i]->get_style_context()->remove_class("selected");
          }
        });

    Glib::signal_timeout().connect(
        [&dsp, address, this]
        {
          m_levels = dsp.getLevel(address);
          return true;
        },
        20);
  }

  std::string Tile::formatTime(long ms) const
  {
    auto s = ms / 1000;
    char txt[256];

    if(s > 0)
      ms -= 1000 * s;

    auto m = s / 60;
    if(m > 0)
      s -= 60 * m;

    auto h = m / 60;
    if(h > 0)
      m -= 60 * h;

    if(h > 0)
      sprintf(txt, "%2ld:%02ld h", h, m);
    else if(m > 0)
      sprintf(txt, "%2ld:%02ld m", m, s);
    else if(s > 0)
      sprintf(txt, "%2ld.%03ld s", s, ms);
    else
      sprintf(txt, "%03ld ms", ms);
    return txt;
  }

  std::array<Gtk::Label*, 64> Tile::addSteps()
  {
    std::array<Gtk::Label*, NUM_STEPS> steps {};

    auto buildStep = [&steps](int i)
    {
      auto label = Gtk::manage(new Gtk::Label(""));
      label->set_name("step-" + std::to_string(i));
      label->get_style_context()->add_class("step");
      steps[i] = label;
      return label;
    };

    for(int i = 0; i < 16; i++)
    {
      attach(*buildStep(i), i, 0);
      attach(*buildStep(i + 16), 16, i);
      attach(*buildStep(i + 32), 16 - i, 16);
      attach(*buildStep(i + 48), 0, 16 - i);
    }
    return steps;
  }

  Gtk::Label* Tile::addDurationLabel()
  {
    auto seconds = Gtk::manage(new Gtk::Label("0.0s"));
    seconds->get_style_context()->add_class("duration");
    seconds->get_style_context()->add_class("seconds");
    seconds->property_halign() = Gtk::ALIGN_START;
    seconds->property_valign() = Gtk::ALIGN_CENTER;
    attach(*seconds, 1, 14, 15, 2);
    return seconds;
  }

  WaveformThumb* Tile::addWaveform(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp,
                                   const Core::Address& address)
  {
    auto waveform = Gtk::manage(new WaveformThumb(core, dsp, address));
    attach(*waveform, 2, 4, 10, 8);
    return waveform;
  }

  Gtk::Label* Tile::addSampleName()
  {
    auto sampleName = Gtk::manage(new Gtk::Label(""));
    sampleName->get_style_context()->add_class("sample-file");
    sampleName->property_halign() = Gtk::ALIGN_START;
    sampleName->property_valign() = Gtk::ALIGN_CENTER;
    sampleName->set_ellipsize(Pango::ELLIPSIZE_START);
    attach(*sampleName, 1, 1, 15, 2);
    return sampleName;
  }

  Gtk::SizeRequestMode Tile::get_request_mode_vfunc() const
  {
    return Gtk::SIZE_REQUEST_CONSTANT_SIZE;
  }

  void Tile::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
  {
    int m = m_size.get_value();
    minimum_height = natural_height = m;
  }

  void Tile::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
  {
    int m = m_size.get_value();
    minimum_width = natural_width = m;
  }
}
