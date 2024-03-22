#pragma once

#include <gtkmm/scrolledwindow.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui
{
  class SharedState;

  namespace Touch
  {
    class Toolboxes : public Gtk::ScrolledWindow
    {
     public:
      explicit Toolboxes(SharedState &sharedUiState, Core::Api::Interface &core);

     private:
      Core::Api::Interface &m_core;
    };
  }
}