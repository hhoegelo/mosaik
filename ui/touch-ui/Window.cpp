#include "Window.h"
#include "ui/touch-ui/sections/tiles/Tiles.h"
#include "ui/touch-ui/sections/toolboxes/Toolboxes.h"
#include "ui/touch-ui/sections/main/Main.h"
#include "ui/touch-ui/sections/mixer/Mixer.h"
#include "LessToCss.h"
#include <gtkmm/grid.h>
#include <iostream>

namespace Ui::Touch
{
  Window::Window(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Ui::Controller& controller)
      : m_toolboxes(std::make_unique<Touch::Toolboxes>(*this, core, controller))
      , m_tiles(std::make_unique<Tiles>(*this, core, dsp, controller))
      , m_main(std::make_unique<Main>(*this))
      , m_mixer(std::make_unique<Mixer>(*this))
  {
#if 0
    set_title("Mosaik");

    if(DEBUG_BUILD)
    {
      std::cout << "Debug Build" << std::endl;
      //set_decorated(false);
      //gtk_window_fullscreen(GTK_WINDOW(Window));
    }

    if(RELEASE_BUILD)
    {
      std::cout << "Release Build" << std::endl;
      fullscreen();
      set_decorated(false);
    }

    //set_resizable(false);
#endif

    set_title("Mosaik");

    if(DEBUG_BUILD)
    {
      std::cout << "Debug Build" << std::endl;
      set_decorated(true);
    }

    if(RELEASE_BUILD)
    {
      std::cout << "Release Build" << std::endl;
      fullscreen();
      set_decorated(false);
    }

    constexpr auto c_screenWidth = 1200;
    constexpr auto c_screenHeight = 720;

    set_default_size(c_screenWidth, c_screenHeight);
    set_size_request(c_screenWidth, c_screenHeight);
    set_resizable(false);
    set_position(Gtk::WindowPosition::WIN_POS_CENTER);

    auto vBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    vBox->get_style_context()->add_class("root");
    auto upperBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
    upperBox->get_style_context()->add_class("upper");
    auto lowerBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
    lowerBox->get_style_context()->add_class("lower");

    vBox->add(*upperBox);
    vBox->add(*lowerBox);

    auto fixed = Gtk::manage(new Gtk::Fixed());
    fixed->add(*m_tiles);
    upperBox->pack_start(*fixed, false, false);
    upperBox->pack_start(*m_toolboxes, Gtk::PackOptions::PACK_EXPAND_WIDGET);

    lowerBox->pack_start(*m_mixer);
    lowerBox->pack_start(*m_main, Gtk::PackOptions::PACK_EXPAND_WIDGET);

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
