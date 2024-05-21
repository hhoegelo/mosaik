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

namespace Ui
{
  class Controller;
}

namespace Ui::Touch
{
  class Tiles;
  class Toolboxes;
  class Mixer;

  class Window : public Gtk::Window, public Interface
  {
   public:
    Window(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp, Ui::Controller &controller);
    ~Window() override;

    ToolboxesInterface &getToolboxes() const override;

    void setColorAdjustmentColor(int r, int g, int b) override;

   private:
    void loadCss();
    bool loadAndMonitor(const char *file);

    std::unique_ptr<Toolboxes> m_toolboxes;
    std::unique_ptr<Tiles> m_tiles;

    Glib::RefPtr<Gtk::StyleContext> m_context;
    Glib::RefPtr<Gio::FileMonitor> m_cssMonitor;
    Glib::RefPtr<Gtk::CssProvider> m_cssProvider;

    Glib::RefPtr<Gtk::CssProvider> m_userStyle;
  };
}
