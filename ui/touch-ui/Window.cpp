#include "Window.h"
#include "ui/touch-ui/sections/tiles/Tiles.h"
#include "ui/touch-ui/sections/toolboxes/Toolboxes.h"
#include "ui/touch-ui/sections/main/Main.h"
#include "ui/touch-ui/sections/mixer/Mixer.h"
#include "LessToCss.h"
#include <gtkmm/grid.h>

namespace Ui::Touch
{
  Window::Window(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp)
      : m_core(core)
      , m_dsp(dsp)
      , m_toolboxes(std::make_unique<Touch::Toolboxes>(*this, m_core))
      , m_tiles(std::make_unique<Tiles>(*this, m_core, m_dsp))
      , m_main(std::make_unique<Main>(*this))
      , m_mixer(std::make_unique<Mixer>(*this))
  {
    set_title("Mosaik");
    set_decorated(true);

    auto vBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    vBox->get_style_context()->add_class("root");
    auto upperBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
    upperBox->get_style_context()->add_class("upper");
    auto lowerBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
    lowerBox->get_style_context()->add_class("lower");

    vBox->add(*upperBox);
    vBox->add(*lowerBox);

    upperBox->add(*m_tiles);
    upperBox->add(*m_toolboxes);
    lowerBox->add(*m_mixer);
    lowerBox->add(*m_main);

    add(*vBox);
    loadCss();
    show_all();
  }

  Window::~Window() = default;

  void Window::loadCss()
  {
    m_context = Gtk::StyleContext::create();
    m_cssProvider = Gtk::CssProvider::create();

    if(!loadAndMonitor(SOURCES_DIR "/ui/touch-ui/styles.less"))
      if(!loadAndMonitor(RESOURCES_DIR "/styles.less"))
        throw std::runtime_error("Could not find or parse style sheet");
  }

  bool Window::loadAndMonitor(const char* fileName)
  {
    try
    {
      auto file = Gio::File::create_for_path(fileName);
      auto less = Glib::file_get_contents(fileName);
      auto css = compileLess(less);
      m_cssProvider->load_from_data(css);
      m_context->add_provider_for_screen(Gdk::Screen::get_default(), m_cssProvider,
                                         GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

      m_cssMonitor = file->monitor_file();
      m_cssMonitor->signal_changed().connect(
          [this, fileName](const auto& file, const auto& other_file, Gio::FileMonitorEvent event_type)
          {
            auto newProvider = Gtk::CssProvider::create();
            try
            {
              auto less = Glib::file_get_contents(fileName);
              auto css = compileLess(less);

              printf("CSS: %s\n", css.c_str());
              
              newProvider->load_from_data(css);
              m_context->remove_provider(m_cssProvider);
              m_cssProvider = newProvider;
              m_context->add_provider_for_screen(Gdk::Screen::get_default(), m_cssProvider,
                                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
            }
            catch(std::exception& e)
            {
              printf("Could not load or parse css file: %s\n", e.what());
            }
          });
    }
    catch(std::exception& e)
    {
      printf("Could not load or parse css file: %s\n", e.what());
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
