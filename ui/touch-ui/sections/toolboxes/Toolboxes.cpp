#include "Toolboxes.h"
#include "ui/Types.h"
#include "TileTools.h"
#include "Waveform.h"
#include "GenericToolbox.h"
#include "ui/touch-ui/tools/WidgetTools.h"

namespace Ui::Touch
{
  class WaveformToolbox : public GenericMaximized<Ui::Toolbox::Waveform>
  {
   public:
    WaveformToolbox(Core::Api::Interface &core, Ui::Controller &controller)
        : GenericMaximized<Ui::Toolbox::Waveform>(controller)
    {
      pack_start(*Gtk::manage(new Touch::Waveform(core)));
    }
  };

  Toolboxes::Toolboxes(Touch::Interface &touch, Core::Api::Interface &core, Ui::Controller &controller)
      : SectionWrapper(touch)
  {
    get_style_context()->add_class("toolboxes");

    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

    box->pack_start(*Gtk::manage(new GenericToolbox<Ui::Toolbox::Global>(*this, controller)));
    box->pack_start(
        *Gtk::manage(new GenericToolbox<Ui::Toolbox::Tile>(*this, controller, new TileTools(*this, core, controller))));
    box->pack_start(*Gtk::manage(
        new GenericToolbox<Ui::Toolbox::Waveform>(*this, controller, new WaveformToolbox(core, controller))));
    box->pack_start(*Gtk::manage(new GenericToolbox<Ui::Toolbox::Steps>(*this, controller)));
    box->pack_start(*Gtk::manage(new GenericToolbox<Ui::Toolbox::Playground>(*this, controller)));
    box->pack_start(*Gtk::manage(new GenericToolbox<Ui::Toolbox::MainPlayground>(*this, controller)));

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
