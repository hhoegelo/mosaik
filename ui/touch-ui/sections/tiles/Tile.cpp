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
#include <gtkmm/levelbar.h>

namespace Ui::Touch
{
  namespace
  {
    double dB(double level)
    {
      if(level <= 0)
        return -80;
      return 20 * std::log10(level);
    }

    std::string toCss(double db)
    {
      int rounded_dB = std::clamp(static_cast<int>(std::round(db / -10.0) * -10), -80, 0);
      return "level-" + std::to_string(std::abs(rounded_dB)) + "db";
    }
  }

  Tile::Tile(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Core::TileId tileId)
      : Glib::ObjectBase("Tile")
      , Gtk::Grid()
      , m_size(*this, "size", 50)
  {
    get_style_context()->add_class("tile");
    set_column_homogeneous(true);
    set_row_homogeneous(true);

    auto steps = addSteps();
    auto sampleName = addSampleName();
    auto waveform = addWaveform(core, tileId);
    auto seconds = addDurationLabel();

    auto drawLevel = [this, d = 0.0f](const Cairo::RefPtr<Cairo::Context>& ctx, float level, float decay) mutable
    {
      d = std::max(d * decay, level);
      ctx->begin_new_path();
      ctx->set_source_rgb(1, 1, 1);
      ctx->set_line_width(2);

      ctx->move_to(0, get_height());
      ctx->line_to(0, (1.0f - d) * get_height());
      ctx->stroke();
    };

    auto levelL = Gtk::manage(new Gtk::DrawingArea());
    levelL->get_style_context()->add_class("level left");
    levelL->signal_draw().connect(
        [drawLevel, this](const Cairo::RefPtr<Cairo::Context>& ctx) mutable
        {
          drawLevel(ctx, std::get<0>(m_levels), 0.98f);
          return true;
        });

    attach(*levelL, 13, 4, 1, 8);

    auto gain = Gtk::manage(new Gtk::DrawingArea());
    gain->get_style_context()->add_class("gain");
    gain->signal_draw().connect(
        [drawLevel, &core, tileId, this](const Cairo::RefPtr<Cairo::Context>& ctx) mutable
        {
          using T = Core::ParameterDescription<Core::ParameterId::Gain>;
          auto v = std::get<float>(core.getParameter(tileId, Core::ParameterId::Gain));
          auto p = (v - T::min) / (T::max - T::min);
          drawLevel(ctx, p, 0.0f);
          return true;
        });
    attach(*gain, 14, 4, 1, 8);

    auto levelR = Gtk::manage(new Gtk::DrawingArea());
    levelR->get_style_context()->add_class("level right");

    levelR->signal_draw().connect(
        [drawLevel, this](const Cairo::RefPtr<Cairo::Context>& ctx) mutable
        {
          drawLevel(ctx, std::get<1>(m_levels), 0.98f);
          return true;
        });

    attach(*levelR, 15, 4, 1, 8);

    m_computations.add([&core, gain, tileId] { gain->queue_draw(); });

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

    runLevelMeterTimer(dsp, tileId, levelL, levelR);
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

  void Tile::runLevelMeterTimer(Dsp::Api::Display::Interface& dsp, Core::TileId tileId, Gtk::DrawingArea* l,
                                Gtk::DrawingArea* r)
  {
    Glib::signal_timeout().connect(
        [&dsp, tileId, l, r, this]
        {
          m_levels = dsp.getLevel(tileId);
          l->queue_draw();
          r->queue_draw();
          return true;
        },
        16);
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
