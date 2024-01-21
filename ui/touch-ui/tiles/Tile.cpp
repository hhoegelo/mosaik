#include "Tile.h"
#include "core/api/Interface.h"
#include "dsp/api/display/Interface.h"
#include "ui/touch-ui/controls/StepButton.h"
#include "ui/touch-ui/controls/FloatScaleButton.h"
#include "ui/touch-ui/controls/Checkbox.h"
#include "ui/touch-ui/controls/Label.h"
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/grid.h>
#include <glibmm/main.h>
#include <gtkmm/scale.h>

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
    auto styles = volume->get_style_context();
    styles->add_class("volume");

    auto volumeChangedConnection = volume->signal_value_changed().connect(
        [&core, volume, tileId]
        { core.setParameter(tileId, Core::ParameterId::Gain, static_cast<float>(volume->get_value())); });

    core.connect(tileId, Core::ParameterId::Gain,
                 [volume, volumeChangedConnection](const Core::ParameterValue& p) mutable
                 {
                   volumeChangedConnection.block();
                   volume->set_value(get<float>(p));
                   volumeChangedConnection.unblock();
                 });

    return volume;
  }

  Tile::Tile(Gtk::Window& wnd, Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Core::TileId tileId)
      : Gtk::Grid()
  {
    get_style_context()->add_class("tileId");
    get_style_context()->add_class("outer-layout");

    attach(*buildHasStepsIndicator(core, tileId), 0, 0, 1, 1);
    attach(*buildPlayIndicator(core, dsp, tileId), 1, 0, 2, 1);

    auto controlsLayout = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_HORIZONTAL));
    pack_start(*controlsLayout, Gtk::PACK_SHRINK);
    controlsLayout->get_style_context()->add_class("controls-layout");

    auto leftControlsLayout = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
    pack_start(*leftControlsLayout, Gtk::PACK_SHRINK);
    leftControlsLayout->get_style_context()->add_class("left-controls-layout");

    auto leftTopControlsLayout = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_HORIZONTAL));
    pack_start(*leftTopControlsLayout, Gtk::PACK_SHRINK);
    leftTopControlsLayout->get_style_context()->add_class("left-top-controls-layout");

    buildHasStepsIndicator(*leftTopControlsLayout, core, tileId);
    buildPlayIndicator(*leftTopControlsLayout, core, dsp, tileId);

    auto bottomControlsLayout = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_HORIZONTAL));
    pack_start(*bottomControlsLayout, Gtk::PACK_SHRINK);
    bottomControlsLayout->get_style_context()->add_class("bottom-controls-layout");

    controlsLayout->pack_start(*leftControlsLayout, Gtk::PACK_SHRINK);
    buildVolumeSlider(*controlsLayout, core, tileId);

    pack_start(*controlsLayout, Gtk::PACK_SHRINK);
    pack_start(*bottomControlsLayout, Gtk::PACK_SHRINK);

    /*
    auto load = Gtk::manage(new Gtk::Button("Load"));
    pack_start(*load, Gtk::PACK_SHRINK);

    pack_start(*Gtk::manage(new FloatScaleButton(core, tileId, Core::ParameterId::Gain, 0.0, 1.0, 0.01, "stock_volume")),
               Gtk::PACK_SHRINK);

    pack_start(*Gtk::manage(
                   new FloatScaleButton(core, tileId, Core::ParameterId::Balance, -1.0, 1.0, 0.02, "pan-start-symbolic")),
               Gtk::PACK_SHRINK);

    pack_start(*Gtk::manage(new Checkbox(core, "Reverse", tileId, Core::ParameterId::Reverse)), Gtk::PACK_SHRINK);
    pack_start(*Gtk::manage(new Checkbox(core, "Mute", tileId, Core::ParameterId::Mute)), Gtk::PACK_SHRINK);

    load->signal_clicked().connect(
        [this, &wnd, &core, tileId]
        {
          Gtk::FileChooserDialog dialog(wnd, "Select audio file...");

          dialog.set_transient_for(wnd);
          dialog.set_modal(true);

          dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
          dialog.add_button("_Open", Gtk::RESPONSE_ACCEPT);

          if(dialog.run() == Gtk::RESPONSE_ACCEPT)
            core.setParameter(Core::TileId { tileId }, Core::ParameterId::SampleFile, dialog.get_filename());
        });
        */
  }
}