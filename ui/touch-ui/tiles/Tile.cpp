#include "Tile.h"
#include "core/api/Interface.h"
#include "dsp/api/display/Interface.h"
#include "ui/touch-ui/controls/FloatScaleButton.h"
#include "ui/touch-ui/controls/Checkbox.h"
#include "ui/touch-ui/controls/Label.h"
#include "dsp/api/control/Interface.h"
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/grid.h>
#include <glibmm/main.h>
#include <gtkmm/scale.h>
#include <gtkmm/drawingarea.h>

#include <iostream>
#include <gtkmm/levelbar.h>

namespace Ui::Touch
{

  Tile::Tile(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Core::TileId tileId)
      : Gtk::Grid()
  {
    get_style_context()->add_class("tile");

    attach(*buildHasStepsIndicator(core, tileId), 0, 0, 1, 1);
    attach(*buildPlayIndicator(core, dsp, tileId), 1, 0, 2, 1);
    attach(*buildVolumeSlider(core, tileId), 3, 0, 1, 4);
    attach(*buildWaveformDisplay(core, tileId), 0, 1, 3, 3);
    attach(*buildDurationInSeconds(core, tileId), 0, 4, 2, 1);
    attach(*buildDurationInSteps(core, tileId), 2, 4, 2, 1);

    m_selectedConnection = core.connect(tileId, Core::ParameterId::Selected,
                                        [this](const Core::ParameterValue& v)
                                        {
                                          if(get<bool>(v))
                                            get_style_context()->add_class("selected");
                                          else
                                            get_style_context()->remove_class("selected");
                                        });
  }

  Gtk::Widget* Tile::buildHasStepsIndicator(Core::Api::Interface& core, Core::TileId tileId)
  {
    constexpr auto hasStepsClass = "has-steps";

    auto hasSteps = Gtk::manage(new Gtk::Label(" [] "));
    auto styles = hasSteps->get_style_context();
    hasSteps->get_style_context()->add_class("has-steps-indicator");

    m_patternConnection
        = core.connect(tileId, Core::ParameterId::Pattern,
                       [styles](const Core::ParameterValue& in) mutable
                       {
                         const auto& p = get<Core::Pattern>(in);
                         bool anyStepSet = std::any_of(p.begin(), p.end(), [](auto step) { return step; });
                         anyStepSet ? styles->add_class(hasStepsClass) : styles->remove_class(hasStepsClass);
                       });

    return hasSteps;
  }

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

  Gtk::Widget* Tile::buildPlayIndicator(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp,
                                        Core::TileId tileId)
  {
    auto play = Gtk::manage(new Gtk::Button());
    auto styles = play->get_style_context();
    styles->add_class("is-playing-indicator");

    m_reverseConnection = core.connect(tileId, Core::ParameterId::Reverse, [play](const Core::ParameterValue& p) mutable
                                       { play->set_label(get<bool>(p) ? "<" : ">"); });

    Glib::signal_timeout().connect(
        [styles, &dsp, tileId, play, oldDB = -80.0]() mutable
        {
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

    return play;
  }

  Gtk::Widget* Tile::buildVolumeSlider(Core::Api::Interface& core, Core::TileId tileId)
  {
    auto volume = Gtk::manage(new Gtk::LevelBar());
    volume->set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);
    volume->set_min_value(0.0);
    volume->set_max_value(1.0);
    volume->set_inverted(true);

    auto styles = volume->get_style_context();
    styles->add_class("volume");

    m_gainConnection = core.connect(tileId, Core::ParameterId::Gain, [volume](const Core::ParameterValue& p) mutable
                                    { volume->set_value(get<float>(p)); });

    return volume;
  }

  Gtk::Widget* Tile::buildWaveformDisplay(Core::Api::Interface& core, Core::TileId tileId)
  {
    auto wf = Gtk::manage(new Gtk::DrawingArea());
    wf->set_size_request(75, 75);

    auto styles = wf->get_style_context();
    styles->add_class("waveform");
    wf->signal_draw().connect(
        [tileId, &core, wf](const Cairo::RefPtr<Cairo::Context>& ctx)
        {
          ctx->set_line_width(1);

          auto samples = core.getSamples(nullptr, tileId);
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

    m_sampleFileConnection = core.connect(tileId, Core::ParameterId::SampleFile,
                                          [wf, &core](const Core::ParameterValue& p) mutable { wf->queue_draw(); });

    wf->add_events(Gdk::EventMask::BUTTON_PRESS_MASK | Gdk::EventMask::POINTER_MOTION_MASK);

    wf->signal_button_press_event().connect(
        [&core, tileId](GdkEventButton*)
        {
          core.setParameter(tileId, Core::ParameterId::Selected, true);
          return false;
        });

    return wf;
  }

  Gtk::Widget* Tile::buildDurationInSeconds(Core::Api::Interface& core, Core::TileId tileId)
  {
    auto wf = Gtk::manage(new Gtk::Label("0.0s"));
    auto styles = wf->get_style_context();
    styles->add_class("duration");
    styles->add_class("seconds");
    return wf;
  }

  Gtk::Widget* Tile::buildDurationInSteps(Core::Api::Interface& core, Core::TileId tileId)
  {
    auto wf = Gtk::manage(new Gtk::Label("0.0"));
    auto styles = wf->get_style_context();
    styles->add_class("duration");
    styles->add_class("steps");
    return wf;
  }
}