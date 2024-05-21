#pragma once

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/styleproperty.h>
#include "ui/Types.h"
#include "ui/touch-ui/Interface.h"

namespace Core::Api
{
  class Interface;
}

namespace Ui
{
  class Controller;
}

namespace Ui::Touch
{
  class Waveform;
  class GlobalTools;
  class TileTools;
  class Steps;

  class Toolboxes : public Gtk::Box, public ToolboxesInterface
  {
   public:
    Toolboxes(Touch::Interface &touch, Core::Api::Interface &core, Ui::Controller &controller);

    WaveformInterface &getWaveform() const override;
    FileBrowserInterface &getFileBrowser() const override;

    Ui::Toolbox getSelectedToolbox() const override;
    void selectToolbox(Ui::Toolbox t) override;

   private:
    Tools::ReactiveVar<::Ui::Toolbox> m_selectedToolbox;
    Gtk::StyleProperty<int> m_height;
  };
}
