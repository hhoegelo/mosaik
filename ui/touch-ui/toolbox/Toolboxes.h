#pragma once

#include <gtkmm/notebook.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui
{
  class SharedState;

  namespace Touch
  {
    class Toolboxes : public Gtk::Notebook
    {
     public:
      explicit Toolboxes(SharedState &sharedUiState, Core::Api::Interface &core);

     private:
      Core::Api::Interface &m_core;
    };
  }
}