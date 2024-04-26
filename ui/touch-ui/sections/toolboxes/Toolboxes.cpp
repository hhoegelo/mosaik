#include "Toolboxes.h"
#include "ui/Types.h"
#include "GlobalTools.h"
#include "TileTools.h"
#include "Toolbox.h"
#include "Waveform.h"
#include "Steps.h"
#include "Playground.h"
#include "MainPlayground.h"
#include <gtkmm/label.h>
#include <gtkmm/eventbox.h>

namespace Ui::Touch
{

  Toolboxes::Toolboxes(Touch::Interface &touch, Core::Api::Interface &core)
      : SectionWrapper(touch)
      , m_core(core)
      , m_box(*Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL)))
  {
    m_tileTools = new TileTools(core);
    auto w = new Touch::Waveform(core);
    m_waveform = w;

    m_box.pack_start(*Gtk::manage(new Toolbox(*this, Ui::Toolbox::Global, "Global",
                                              new Gtk::Label("I am minimized global toolbox"), new GlobalTools(core))));

    m_box.pack_start(*Gtk::manage(
        new Toolbox(*this, Ui::Toolbox::Tile, "Tile", new Gtk::Label("I am minimized Tile toolbox"), m_tileTools)));

    m_box.pack_start(*Gtk::manage(
        new Toolbox(*this, Ui::Toolbox::Waveform, "Waveform", new Gtk::Label("I am minimized waveform toolbox"), w)));

    m_box.pack_start(*Gtk::manage(new Toolbox(*this, Ui::Toolbox::Steps, "Steps",
                                              new Gtk::Label("I am minimized Steps toolbox"), new Steps(core))));

    m_box.pack_start(
        *Gtk::manage(new Toolbox(*this, Ui::Toolbox::Playground, "Playground",
                                 new Gtk::Label("I am minimized Playground toolbox"), new Playground(core))));

    m_box.pack_start(
        *Gtk::manage(new Toolbox(*this, Ui::Toolbox::MainPlayground, "MainPlayground",
                                 new Gtk::Label("I am minimized MainPlayground toolbox"), new MainPlayground(core))));

    get_style_context()->add_class("toolboxes");

    auto scroller = Gtk::manage(new Gtk::ScrolledWindow());
    scroller->add(m_box);
    add(*scroller);

    set_hexpand();
    m_box.set_hexpand_set();
    scroller->set_hexpand();

    scroller->property_hscrollbar_policy() = Gtk::PolicyType::POLICY_NEVER;
    scroller->set_propagate_natural_width();
  }

  Ui::Toolbox Toolboxes::getSelectedToolbox() const
  {
    return m_selectedToolbox;
  }

  WaveformInterface &Toolboxes::getWaveform() const
  {
    return *m_waveform;
  }

  FileBrowserInterface &Toolboxes::getFileBrowser() const
  {
    return m_tileTools->getFileBrowser();
  }

  void Toolboxes::selectToolbox(Ui::Toolbox t)
  {
    m_selectedToolbox = t;
  }
}
