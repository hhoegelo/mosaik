#include <gtkmm/filechooserdialog.h>
#include "Tile.h"
#include "StepButton.h"
#include "core/api/Interface.h"
#include "FloatScaleButton.h"
#include "Checkbox.h"

namespace Ui::Touch
{
  Tile::Tile(Gtk::Window& wnd, Core::Api::Interface& core, int tile)
      : Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 2)
  {
    auto load = Gtk::manage(new Gtk::Button("Load"));
    pack_start(*load, Gtk::PACK_SHRINK);

    pack_start(
        *Gtk::manage(new FloatScaleButton(core, tile, Core::ParameterId::Gain, 0.0, 1.0, 0.01, "audio-speakers")),
        Gtk::PACK_SHRINK);

    pack_start(*Gtk::manage(
                   new FloatScaleButton(core, tile, Core::ParameterId::Balance, -1.0, 1.0, 0.02, "pan-down-symbolic")),
               Gtk::PACK_SHRINK);

    pack_start(*Gtk::manage(new Checkbox(core, "Reverse", tile, Core::ParameterId::Reverse)), Gtk::PACK_SHRINK);
    pack_start(*Gtk::manage(new Checkbox(core, "Mute", tile, Core::ParameterId::Mute)), Gtk::PACK_SHRINK);

    for(auto s = 0; s < NUM_STEPS; s++)
    {
      auto step = Gtk::manage(new StepButton(core, tile, s));
      pack_start(*step, Gtk::PACK_SHRINK);
    }

    load->signal_clicked().connect(
        [this, &wnd, &core, tile]
        {
          Gtk::FileChooserDialog dialog(wnd, "Select audio file...");

          dialog.set_transient_for(wnd);
          dialog.set_modal(true);

          dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
          dialog.add_button("_Open", Gtk::RESPONSE_ACCEPT);

          if(dialog.run() == Gtk::RESPONSE_ACCEPT)
            core.setParameter(Core::TileId { tile }, Core::ParameterId::SampleFile, dialog.get_filename());
        });
  }
}