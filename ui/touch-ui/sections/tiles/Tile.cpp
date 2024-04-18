#include "Tile.h"
#include "core/api/Interface.h"
#include "dsp/api/display/Interface.h"
#include "dsp/api/control/Interface.h"
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <glibmm/main.h>
#include <gtkmm/scale.h>
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
      : Gtk::Grid()
  {
    get_style_context()->add_class("tile");

    auto hasSteps = Gtk::manage(new Gtk::Label(" [] "));
    hasSteps->get_style_context()->add_class("has-steps-indicator");
    attach(*hasSteps, 0, 0, 1, 1);

    auto reverse = Gtk::manage(new Gtk::Label(" > "));
    reverse->get_style_context()->add_class("is-playing-indicator");
    attach(*reverse, 1, 0, 2, 1);

    using GainDesc = Core::ParameterDescription<Core::ParameterId::Gain>;

    auto volume = Gtk::manage(new Gtk::LevelBar());
    volume->set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);
    volume->set_min_value(0.0);
    volume->set_max_value(GainDesc::max - GainDesc::min);
    volume->set_inverted(true);
    volume->get_style_context()->add_class("volume");
    attach(*volume, 3, 0, 1, 4);

    auto waveform = buildWaveformDisplay(core, tileId);
    attach(*waveform, 0, 1, 12, 3);

    auto seconds = Gtk::manage(new Gtk::Label("0.0s"));
    seconds->get_style_context()->add_class("duration");
    seconds->get_style_context()->add_class("seconds");
    attach(*seconds, 0, 4, 2, 1);

    auto steps = Gtk::manage(new Gtk::Label("0.0"));
    steps->get_style_context()->add_class("duration");
    steps->get_style_context()->add_class("steps");
    attach(*steps, 2, 4, 2, 1);

    m_computations.add(
        [&core, tileId, hasSteps]()
        {
          constexpr auto hasStepsClass = "has-steps";
          auto styles = hasSteps->get_style_context();
          hasSteps->get_style_context()->add_class("has-steps-indicator");
          auto pattern = std::get<Core::Pattern>(core.getParameter(tileId, Core::ParameterId::Pattern));
          bool anyStepSet = std::any_of(pattern.begin(), pattern.end(), [](auto step) { return step; });
          anyStepSet ? styles->add_class(hasStepsClass) : styles->remove_class(hasStepsClass);
        });

    m_computations.add(
        [&core, tileId, reverse]()
        { reverse->set_label(std::get<bool>(core.getParameter(tileId, Core::ParameterId::Reverse)) ? " < " : " > "); });

    m_computations.add(
        [&core, tileId, volume]()
        { volume->set_value(std::get<float>(core.getParameter(tileId, Core::ParameterId::Gain)) - GainDesc::min); });

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

    runLevelMeterTimer(dsp, tileId, reverse);
  }

  Gtk::Widget* Tile::buildWaveformDisplay(Core::Api::Interface& core, Core::TileId tileId)
  {
    auto wf = Gtk::manage(new Gtk::DrawingArea());
    wf->set_size_request(60, 60);

    auto styles = wf->get_style_context();
    styles->add_class("waveform");
    wf->signal_draw().connect(
        [tileId, &core, wf](const Cairo::RefPtr<Cairo::Context>& ctx)
        {
          ctx->set_line_width(1);

          auto samples = core.getSamples(tileId);
          auto w = wf->get_width();
          auto h = wf->get_height();
          auto adv = std::max<double>(1, samples.get()->size() / static_cast<double>(w));
          auto frame = 0.0;

          for(size_t i = 0; i < w; i++)
          {
            float v = 0;

            for(size_t a = 0; a < static_cast<size_t>(adv); a++)
            {
              auto idx = static_cast<size_t>(std::round(frame + a));
              if(idx < samples->size())
              {
                v = std::max(v, std::abs(std::max(samples->data()[idx].left, samples->data()[idx].right)));
              }
            }

            ctx->move_to(i, h / 2 + v * h / 2);
            ctx->line_to(i, h / 2 - v * h / 2);
            frame += adv;
          }

          ctx->stroke();
          return true;
        });

    wf->add_events(Gdk::EventMask::BUTTON_PRESS_MASK | Gdk::EventMask::POINTER_MOTION_MASK);

    wf->signal_button_press_event().connect(
        [&core, tileId](GdkEventButton*)
        {
          core.setParameter(tileId, Core::ParameterId::Selected, true);
          return false;
        });

    return wf;
  }

  void Tile::runLevelMeterTimer(Dsp::Api::Display::Interface& dsp, Core::TileId tileId, Gtk::Label* reverse)
  {
    Glib::signal_timeout().connect(
        [&dsp, tileId, reverse, oldDB = -80.0]() mutable
        {
          auto styles = reverse->get_style_context();
          auto decay = 0.0;
          auto db = dB(dsp.getCurrentTileLevel(tileId));
          db = decay * oldDB + (1.0 - decay) * db;
          oldDB = db;

          auto cssClass = toCss(db);

          for(const auto& color : styles->list_classes())
          {
            if(color.find("level-") == 0)
              if(color != cssClass)
                styles->remove_class(color);
          }
          styles->add_class(cssClass);

          return true;
        },
        16);
  }
}
