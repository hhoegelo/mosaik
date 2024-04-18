#include "Window.h"
#include "ui/touch-ui/sections/tiles/Tiles.h"
#include "ui/touch-ui/sections/toolboxes/Toolboxes.h"
#include <gtkmm/grid.h>

namespace Ui::Touch
{
  Window::Window(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp)
      : m_core(core)
      , m_dsp(dsp)
      , m_toolboxes(std::make_unique<Touch::Toolboxes>(*this, m_core))
      , m_tiles(std::make_unique<Tiles>(*this, m_core, m_dsp))
  {
    set_title("Mosaik");
    set_border_width(10);
    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));

    box->pack_start(*m_tiles, false, false);
    box->pack_start(*m_toolboxes, true, true);

    add(*box);
    loadCss();
    show_all();
  }

  Window::~Window() = default;

  void Window::loadCss()
  {
    m_context = Gtk::StyleContext::create();
    m_cssProvider = Gtk::CssProvider::create();

    if(!loadAndMonitor(SOURCES_DIR "/ui/touch-ui/styles.css"))
      if(!loadAndMonitor(RESOURCES_DIR "/styles.css"))
        throw std::runtime_error("Could not find style sheet");
  }

  bool Window::loadAndMonitor(const char* fileName)
  {
    try
    {
      auto file = Gio::File::create_for_path(fileName);
      m_cssProvider->load_from_path(fileName);
      m_context->add_provider_for_screen(Gdk::Screen::get_default(), m_cssProvider,
                                         GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

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
              printf("Could not load or parse css file\n");
            }
          });
    }
    catch(...)
    {
      return false;
    }
    return true;
  }

  ToolboxesInterface& Window::getToolboxes() const
  {
    return *m_toolboxes;
  }

  Section Window::getCurrentSection() const
  {
    return m_section;
  }

  void Window::selectSection(Section s)
  {
    m_section = s;
  }
}
