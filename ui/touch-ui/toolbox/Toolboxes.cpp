#include "Toolboxes.h"
#include "Pattern.h"
#include <core/api/Interface.h>
#include <gtkmm/filechooserwidget.h>

namespace Ui::Touch
{
  Toolboxes::Toolboxes(Core::Api::Interface &core)
  {
    auto fileBrowser = Gtk::manage(new Gtk::FileChooserWidget(Gtk::FILE_CHOOSER_ACTION_OPEN));
    fileBrowser->signal_file_activated().connect(
        [fileBrowser, &core]
        {
          for(auto tileId : core.getSelectedTiles())
            core.setParameter(tileId, Core::ParameterId::SampleFile, fileBrowser->get_filename());
        });

    this->append_page(*fileBrowser, "Browse");

    auto pattern = Gtk::manage(new Pattern(core));
    this->append_page(*pattern, "Pattern");
  }
}
