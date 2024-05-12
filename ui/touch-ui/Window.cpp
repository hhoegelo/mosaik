#include "Window.h"
#include "ui/touch-ui/sections/tiles/Tiles.h"
#include "ui/touch-ui/sections/toolboxes/Toolboxes.h"
#include "ui/touch-ui/sections/mixer/Mixer.h"
#include "LessToCss.h"
#include <gtkmm/grid.h>
#include <iostream>
#include <ui/Controller.h>

namespace Ui::Touch
{
  Window::Window(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Ui::Controller& controller)
      : m_toolboxes(std::make_unique<Touch::Toolboxes>(*this, core, controller))
      , m_tiles(std::make_unique<Tiles>(*this, core, dsp, controller))
      , m_mixer(std::make_unique<Mixer>(*this, core, dsp, controller))
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

    controller.run();

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

    auto fixedBox = Gtk::manage(new Gtk::Fixed());
    auto hBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
    hBox->get_style_context()->add_class("root");

    auto left = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    left->get_style_context()->add_class("left");

    hBox->pack_start(*left, false, false);
    hBox->pack_start(*m_toolboxes, true, false);

    auto fixedTiles = Gtk::manage(new Gtk::Fixed());
    fixedTiles->add(*m_tiles);
    left->pack_start(*fixedTiles, false, false);
    auto fixedMixer = Gtk::manage(new Gtk::Fixed());
    fixedMixer->add(*m_mixer);
    left->pack_start(*fixedMixer, false, false);

    fixedBox->add(*hBox);
    add(*fixedBox);
    loadCss();
    show_all();

    add_events(Gdk::KEY_PRESS_MASK);
    signal_key_press_event().connect(
        [&](GdkEventKey* e)
        {
          if(e->keyval == GDK_KEY_Escape)
            get_application()->quit();
          if(e->keyval == GDK_KEY_i)
            core.init();

          return false;
        });
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
