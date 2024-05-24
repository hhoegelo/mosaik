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
      : Glib::ObjectBase("Toolboxes")
      , m_height(*this, "height", 50)
  {
    get_style_context()->add_class("toolboxes");

    auto scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
    scroll->set_propagate_natural_width();
    scroll->set_policy(Gtk::PolicyType::POLICY_NEVER, Gtk::PolicyType::POLICY_AUTOMATIC);

    signal_style_updated().connect([this] { set_size_request(-1, m_height); });

    auto box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL);

    box->pack_start(*Gtk::make_managed<GenericToolbox<Ui::Toolbox::Global>>(*this, controller));
    box->pack_start(*Gtk::make_managed<GenericToolbox<Ui::Toolbox::Tile>>(*this, controller,
                                                                          new TileTools(*this, core, controller)));
    box->pack_start(*Gtk::make_managed<GenericToolbox<Ui::Toolbox::Waveform>>(*this, controller,
                                                                              new WaveformToolbox(core, controller)));
    box->pack_start(*Gtk::make_managed<GenericToolbox<Ui::Toolbox::Steps>>(*this, controller));
    box->pack_start(*Gtk::make_managed<GenericToolbox<Ui::Toolbox::Mute>>(*this, controller));
    box->pack_start(*Gtk::make_managed<GenericToolbox<Ui::Toolbox::Reverb>>(*this, controller));
    box->pack_start(*Gtk::make_managed<GenericToolbox<Ui::Toolbox::Snapshots>>(*this, controller));

    //auto colorAdjust = Gtk::make_managed<GenericToolbox<Ui::Toolbox::ColorAdjust>>(*this, controller);
    //colorAdjust->get_style_context()->add_class("color-adjust");
    //box->pack_start(*colorAdjust);

    scroll->add(*box);
    add(*scroll);

    set_valign(Gtk::Align::ALIGN_FILL);
    set_resize_mode(Gtk::ResizeMode::RESIZE_QUEUE);
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
