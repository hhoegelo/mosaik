#include "Tile.h"
#include "core/api/Interface.h"
#include "dsp/api/display/Interface.h"
#include "ui/touch-ui/controls/StepButton.h"
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

namespace Ui::Touch
{
  static Gtk::Widget* buildHasStepsIndicator(Core::Api::Interface& core, Core::TileId tileId)
  {
    constexpr auto hasStepsClass = "has-steps";

    auto hasSteps = Gtk::manage(new Gtk::Label(" [] "));
    auto styles = hasSteps->get_style_context();
    hasSteps->get_style_context()->add_class("has-steps-indicator");

    core.connect(tileId, Core::ParameterId::Pattern,
                 [styles](const Core::ParameterValue& in) mutable
                 {
                   const auto& p = get<Core::Pattern>(in);
                   bool anyStepSet = std::any_of(p.begin(), p.end(), [](auto step) { return step; });
                   anyStepSet ? styles->add_class(hasStepsClass) : styles->remove_class(hasStepsClass);
                 });

    return hasSteps;
  }

  static Gtk::Widget* buildPlayIndicator(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp,
                                         Core::TileId tileId)
  {
    constexpr auto isPlayingClass = "is-playing";

    auto play = Gtk::manage(new Gtk::Button());
    auto styles = play->get_style_context();
    styles->add_class("is-playing-indicator");

    auto playPressedConnection = play->signal_clicked().connect(
        [&core, tileId]
        {
          core.setParameter(tileId, Core::ParameterId::Reverse,
                            !get<bool>(core.getParameter(tileId, Core::ParameterId::Reverse)));
        });

    core.connect(tileId, Core::ParameterId::Reverse,
                 [play, playPressedConnection](const Core::ParameterValue& p) mutable
                 {
                   playPressedConnection.block();
                   play->set_label(get<bool>(p) ? "<" : ">");
                   playPressedConnection.unblock();
                 });

    Glib::signal_timeout().connect(
        [styles, &dsp, tileId]
        {
          dsp.isTileCurrentlyPlaying(tileId) ? styles->add_class(isPlayingClass) : styles->remove_class(isPlayingClass);
          return true;
        },
        16);

    return play;
  }

  static Gtk::Widget* buildVolumeSlider(Core::Api::Interface& core, Core::TileId tileId)
  {
    auto volume = Gtk::manage(new Gtk::Scale(Gtk::Orientation::ORIENTATION_VERTICAL));
    volume->set_range(0.0, 1.0);
    volume->set_inverted(true);
    volume->set_increments(0.01, 0.05);
    volume->set_draw_value(false);

    auto styles = volume->get_style_context();
    styles->add_class("volume");

    auto volumeChangedConnection = volume->signal_change_value().connect(
        [&core, volume, tileId](Gtk::ScrollType, double v)
        {
          core.setParameter(tileId, Core::ParameterId::Gain, static_cast<float>(v));
          return true;
        });

    core.connect(tileId, Core::ParameterId::Gain,
                 [volume, volumeChangedConnection](const Core::ParameterValue& p) mutable
                 {
                   volumeChangedConnection.block();
                   volume->set_value(get<float>(p));
                   volumeChangedConnection.unblock();
                 });

    return volume;
  }

  static Gtk::Widget* buildWaveformDisplay(Core::Api::Interface& core, Core::TileId tileId)
  {
    auto wf = Gtk::manage(new Gtk::DrawingArea());
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

    core.connect(tileId, Core::ParameterId::SampleFile,
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

  static Gtk::Widget* buildDurationInSeconds(Core::Api::Interface& core, Core::TileId tileId)
  {
    auto wf = Gtk::manage(new Gtk::Label("0.0s"));
    auto styles = wf->get_style_context();
    styles->add_class("duration");
    styles->add_class("seconds");
    return wf;
  }

  static Gtk::Widget* buildDurationInSteps(Core::Api::Interface& core, Core::TileId tileId)
  {
    auto wf = Gtk::manage(new Gtk::Label("0.0"));
    auto styles = wf->get_style_context();
    styles->add_class("duration");
    styles->add_class("steps");
    return wf;
  }

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

    core.connect(tileId, Core::ParameterId::Selected,
                 [this](const Core::ParameterValue& v)
                 {
                   if(get<bool>(v))
                     get_style_context()->add_class("selected");
                   else
                     get_style_context()->remove_class("selected");
                 });
  }
}