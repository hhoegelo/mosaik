#include "Window.h"
#include <core/api/Interface.h>

namespace Ui
{
  namespace Touch
  {
    Window::Window(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp)
        : m_box(Gtk::Orientation::VERTICAL)
        , m_button1("Btn 1")
        , m_button2("Btn 2")
    {
      set_title("Mosaik");
      set_default_size(500, 500);

      set_child(m_box);
      m_box.append(m_button1);
      m_box.append(m_button2);

      m_button1.signal_clicked().connect([this, &core] {});
    }

    Window::~Window()
    {
    }
  }
}