#include "Window.h"
#include <core/api/Interface.h>

namespace Ui
{
  namespace Touch
  {
    Window::Window(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp)
    {
      set_title("Mosaik");
      set_border_width(10);

      auto hbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
      add(*hbox);

      auto load = Gtk::manage(new Gtk::Button("Load Sample"));
      hbox->pack_start(*load, Gtk::PACK_SHRINK);

      auto trigger = Gtk::manage(new Gtk::Button("(Re-)Trigger"));
      hbox->pack_start(*trigger, Gtk::PACK_SHRINK);

      show_all();

      load->signal_clicked().connect(
          [this, &core]
          {
            Gtk::FileChooserDialog dialog(*this, "Select audio file...");

            dialog.set_transient_for(*this);
            dialog.set_modal(true);

            dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
            dialog.add_button("_Open", Gtk::RESPONSE_ACCEPT);

            if(dialog.run() == Gtk::RESPONSE_ACCEPT)
            {
              core.loadSample(0, 0, dialog.get_filename());
            }
          });

      trigger->signal_clicked().connect([this, &core] { core.trigger(0, 0); });
    }
  }
}