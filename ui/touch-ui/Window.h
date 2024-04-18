#pragma once

#include <ui/touch-ui/Interface.h>
#include <gtkmm-3.0/gtkmm.h>

namespace Core::Api
{
  class Interface;
}

namespace Dsp::Api::Display
{
  class Interface;
}

namespace Ui::Touch
{
  class Tiles;
  class Toolboxes;

  class Window : public Gtk::Window, public Interface
  {
   public:
    Window(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp);
    ~Window() override;

    ToolboxesInterface &getToolboxes() const override;
    Section getCurrentSection() const;
    void selectSection(Section s);

   private:
    void loadCss();
    bool loadAndMonitor(const char *file);

    Core::Api::Interface &m_core;
    Dsp::Api::Display::Interface &m_dsp;

    Tools::ReactiveVar<Section> m_section { Section::Tiles };

    // Sections
    std::unique_ptr<Tiles> m_tiles;
    std::unique_ptr<Toolboxes> m_toolboxes;

    Glib::RefPtr<Gtk::StyleContext> m_context;
    Glib::RefPtr<Gio::FileMonitor> m_cssMonitor;
    Glib::RefPtr<Gtk::CssProvider> m_cssProvider;
  };
}
