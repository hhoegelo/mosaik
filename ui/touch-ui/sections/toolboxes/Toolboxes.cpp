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
  template <typename T>
  static T *addToolbox(Tools::ReactiveVar<::Ui::Toolbox> &var, Ui::Toolbox s, Gtk::Box *box, const std::string &title,
                       T *child)
  {
    child = Gtk::manage(child);

    auto events = Gtk::manage(new Gtk::EventBox());
    auto section = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    auto label = Gtk::manage(new Gtk::Label(title));
    label->get_style_context()->add_class("header");
    section->pack_start(*label, false, true);
    section->pack_start(*child, true, true);
    section->get_style_context()->add_class("section");
    events->add(*section);

    box->pack_start(*events, true, true);
    box->set_hexpand();

    events->add_events(Gdk::BUTTON_PRESS_MASK);

    events->signal_button_press_event().connect(
        [&var, s](GdkEventButton *event)
        {
          var = s;
          return false;
        });

    return child;
  }

  Toolboxes::Toolboxes(Touch::Interface &touch, Core::Api::Interface &core)
      : SectionWrapper(touch)
      , m_core(core)
      , m_box(*Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL)))
      , m_tileTools(*addToolbox(m_selectedToolbox, Ui::Toolbox::Tile, &m_box, "Tile", new TileTools(core)))
      , m_waveform(*addToolbox(m_selectedToolbox, Ui::Toolbox::Waveform, &m_box, "Wave", new Touch::Waveform(core)))
  {
    {
      auto a = Gtk::manage(new Toolbox(*this, Ui::Toolbox::Global, "Toolbox: Global",
                                       new Gtk::Label("I am minimized global toolbox"),
                                       new Gtk::Label("I am maximized global toolbox")));

      auto b = Gtk::manage(new Toolbox(*this, Ui::Toolbox::MainPlayground, "Toolbox: Main Playground",
                                       new Gtk::Label("I am minimized main playground"),
                                       new Gtk::Label("I am maximized main playground")));

      auto c = Gtk::manage(new Toolbox(*this, Ui::Toolbox::Steps, "Toolbox: Steps",
                                       new Gtk::Label("I am minimized steps"), new Gtk::Label("I am maximized steps")));
      m_box.pack_start(*a);
      m_box.pack_start(*b);
      m_box.pack_start(*c);
    }

    addToolbox(m_selectedToolbox, Ui::Toolbox::Global, &m_box, "Global", new GlobalTools(core));
    addToolbox(m_selectedToolbox, Ui::Toolbox::Playground, &m_box, "Playground", new Playground(core));
    addToolbox(m_selectedToolbox, Ui::Toolbox::Steps, &m_box, "Steps", new Steps(core));
    addToolbox(m_selectedToolbox, Ui::Toolbox::MainPlayground, &m_box, "Main Playground", new MainPlayground(core));

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
    return m_waveform;
  }

  FileBrowserInterface &Toolboxes::getFileBrowser() const
  {
    return m_tileTools.getFileBrowser();
  }

  void Toolboxes::selectToolbox(Ui::Toolbox t)
  {
    m_selectedToolbox = t;
  }

}
