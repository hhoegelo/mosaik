#include "Tile.h"
#include "core/api/Interface.h"
#include "dsp/api/display/Interface.h"
#include "WaveformThumb.h"
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <glibmm/main.h>
#include <gtkmm/drawingarea.h>

#include <iostream>
#include "LevelMeter.h"

namespace Ui::Touch
{
  namespace
  {
    float ampToLevelMeter(float amp)
    {
      auto minDb = -72.f;

      if(amp == 0.f)
        return 0.f;

      auto db = log10f(amp) * 20.f;
      return std::clamp(1.f - db / minDb, 0.f, 1.f);
    }
  }

  Tile::Tile(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Core::TileId tileId)
      : Glib::ObjectBase("Tile")
      , Gtk::Grid()
      , m_size(*this, "size", 50)
  {
    constexpr auto levelMeterDecay = 0.98f;

    get_style_context()->add_class("tile");
    set_column_homogeneous(true);
    set_row_homogeneous(true);

    auto steps = addSteps();
    auto sampleName = addSampleName();
    auto waveform = addWaveform(core, tileId);
    auto seconds = addDurationLabel();

    attach(*Gtk::manage(new LevelMeter(
               "level left", [this] { return ampToLevelMeter(std::get<0>(m_levels.get())); }, levelMeterDecay)),
           13, 4, 1, 8);

    attach(*Gtk::manage(new LevelMeter("gain",
                                       [this, &core, tileId]
                                       {
                                         using T = Core::ParameterDescription<Core::ParameterId::Gain>;
                                         auto v = std::get<float>(core.getParameter(tileId, Core::ParameterId::Gain));
                                         return (v - T::min) / (T::max - T::min);
                                       })),
           14, 4, 1, 8);

    attach(*Gtk::manage(new LevelMeter(
               "level right", [this] { return ampToLevelMeter(std::get<1>(m_levels.get())); }, levelMeterDecay)),
           15, 4, 1, 8);

    m_computations.add([&core, tileId, sampleName]()
                       { sampleName->set_label(core.getParameterDisplay(tileId, Core::ParameterId::SampleFile)); });

    m_computations.add(
        [this, &core, &dsp, tileId, seconds]()
        {
          auto file = std::get<Core::Path>(core.getParameter(tileId, Core::ParameterId::SampleFile));
          auto ms = dsp.getDuration(file).count();
          seconds->set_label(formatTime(ms));
        });

    m_computations.add(
        [&core, tileId, this]()
        {
          if(std::get<bool>(core.getParameter(tileId, Core::ParameterId::Selected)))
            get_style_context()->add_class("selected");
          else
            get_style_context()->remove_class("selected");
        });

    m_computations.add(
        [&core, tileId, waveform]()
        {
          auto _ = core.getParameter(tileId, Core::ParameterId::SampleFile);
          waveform->queue_draw();
        });

    m_computations.add(
        [&core, tileId, this, steps]()
        {
          auto pattern = std::get<Core::Pattern>(core.getParameter(tileId, Core::ParameterId::Pattern));

          for(size_t i = 0; i < 64; i++)
          {
            if(pattern[i])
              steps[i]->get_style_context()->add_class("selected");
            else
              steps[i]->get_style_context()->remove_class("selected");
          }
        });

    Glib::signal_timeout().connect(
        [&dsp, tileId, this]
        {
          m_levels = dsp.getLevel(tileId);
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
      sprintf(txt, "%2d:%02d h", h, m);
    else if(m > 0)
      sprintf(txt, "%2d:%02d m", m, s);
    else if(s > 0)
      sprintf(txt, "%2d.%03d s", s, ms);
    else
      sprintf(txt, "%03d ms", ms);
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

  WaveformThumb* Tile::addWaveform(Core::Api::Interface& core, const Core::TileId& tileId)
  {
    auto waveform = Gtk::manage(new WaveformThumb(core, tileId));
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

  void Tile::on_size_allocate(Gtk::Allocation& allocation)
  {
    auto m = std::min(allocation.get_height(), allocation.get_width());
    allocation.set_height(m);
    allocation.set_width(m);
    Gtk::Grid::on_size_allocate(allocation);
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
