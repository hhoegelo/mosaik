#include "Window.h"
#include "ui/touch-ui/tiles/Tiles.h"
#include "ui/touch-ui/toolbox/Toolboxes.h"
#include <gtkmm/grid.h>

namespace Ui::Touch
{
  Window::Window(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp)
      : m_core(core)
      , m_dsp(dsp)
  {
    build();
  }

  Window::~Window() = default;

  bool Window::loadAndMonitor(const char* fileName)
  {
    try
    {
      m_cssProvider->load_from_path(fileName);

      auto file = Gio::File::create_for_path(fileName);

      m_cssMonitor = file->monitor_file();
      m_cssMonitor->signal_changed().connect(
          [this, fileName](const auto& file, const auto& other_file, Gio::FileMonitorEvent event_type)
          {
            auto newProvider = Gtk::CssProvider::create();
            try
            {
              newProvider->load_from_path(fileName);
              m_context->remove_provider(m_cssProvider);
              m_cssProvider = newProvider;
              m_context->add_provider_for_screen(Gdk::Screen::get_default(), m_cssProvider,
                                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
            }
            catch(...)
            {
            }
          });
    }
    catch(...)
    {
      return false;
    }
    return true;
  }

  void Window::build()
  {
    m_context = Gtk::StyleContext::create();
    m_cssProvider = Gtk::CssProvider::create();

    if(!loadAndMonitor(SOURCES_DIR "/ui/touch-ui/styles.css"))
      if(!loadAndMonitor(RESOURCES_DIR "/styles.css"))
        throw std::runtime_error("Could not find style sheet");

    m_context->add_provider_for_screen(Gdk::Screen::get_default(), m_cssProvider,
                                       GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    set_title("Mosaik");
    set_border_width(10);
    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));

    m_tiles = new Tiles(m_core, m_dsp);
    box->pack_start(*m_tiles, false, false);

    m_toolboxes = new Touch::Toolboxes(m_core);
    box->pack_start(*m_toolboxes, true, true);

    add(*box);
    show_all();
  }

  ToolboxesInterface& Window::getToolboxes() const
  {
    return *m_toolboxes;
  }
}
