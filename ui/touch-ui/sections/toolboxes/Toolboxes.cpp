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
#include <gtkmm/fixed.h>

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

    auto add = [&](Gtk::Widget *w) { m_box.pack_start(*Gtk::manage(w), Gtk::PACK_SHRINK); };

    add(new Toolbox(*this, Ui::Toolbox::Global, "Global", new Gtk::Label("I am minimized global toolbox"),
                    new GlobalTools(core)));

    add(new Toolbox(*this, Ui::Toolbox::Tile, "Tile", new Gtk::Label("I am minimized Tile toolbox"), m_tileTools));

    add(new Toolbox(*this, Ui::Toolbox::Waveform, "Waveform", new Gtk::Label("I am minimized waveform toolbox"), w));

    add(new Toolbox(*this, Ui::Toolbox::Steps, "Steps", new Gtk::Label("I am minimized Steps toolbox"),
                    new Steps(core)));

    add(new Toolbox(*this, Ui::Toolbox::Playground, "Playground", new Gtk::Label("I am minimized Playground toolbox"),
                    new Playground(core)));

    add(new Toolbox(*this, Ui::Toolbox::MainPlayground, "MainPlayground",
                    new Gtk::Label("I am minimized MainPlayground toolbox"), new MainPlayground(core)));

    get_style_context()->add_class("toolboxes");

    this->add(m_box);
    m_box.set_hexpand();
    set_valign(Gtk::Align::ALIGN_START);
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
