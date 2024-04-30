#include "Toolboxes.h"
#include "ui/Types.h"
#include "GlobalTools.h"
#include "TileTools.h"
#include "Waveform.h"
#include "GenericToolbox.h"
#include "ui/touch-ui/tools/WidgetTools.h"

namespace Ui::Touch
{

  Toolboxes::Toolboxes(Touch::Interface &touch, Core::Api::Interface &core)
      : SectionWrapper(touch)
  {
    get_style_context()->add_class("toolboxes");

    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

    box->set_hexpand();
    box->pack_start(*Gtk::manage(new GenericToolbox<Ui::Toolbox::Global>(*this, core)));
    box->pack_start(*Gtk::manage(new GenericToolbox<Ui::Toolbox::Tile>(*this, core, new TileTools(core))));
    box->pack_start(*Gtk::manage(new GenericToolbox<Ui::Toolbox::Waveform>(*this, core, new Touch::Waveform(core))));
    box->pack_start(*Gtk::manage(new GenericToolbox<Ui::Toolbox::Steps>(*this, core)));
    box->pack_start(*Gtk::manage(new GenericToolbox<Ui::Toolbox::Playground>(*this, core)));
    box->pack_start(*Gtk::manage(new GenericToolbox<Ui::Toolbox::MainPlayground>(*this, core)));

    add(*box);

    set_valign(Gtk::Align::ALIGN_START);
  }

  Ui::Toolbox Toolboxes::getSelectedToolbox() const
  {
    return m_selectedToolbox;
  }

  WaveformInterface &Toolboxes::getWaveform() const
  {
    return *findChildWidget<WaveformInterface>(this);
  }

  FileBrowserInterface &Toolboxes::getFileBrowser() const
  {
    return *findChildWidget<FileBrowserInterface>(this);
  }

  void Toolboxes::selectToolbox(Ui::Toolbox t)
  {
    m_selectedToolbox = t;
  }
}
