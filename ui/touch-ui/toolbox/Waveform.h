#pragma once

#include <gtkmm/drawingarea.h>

namespace Core::Api
{
  class Interface;
  class Computation;
}

namespace Ui
{
  class SharedState;

  namespace Touch
  {
    class Waveform : public Gtk::DrawingArea
    {
     public:
      explicit Waveform(Ui::SharedState &sharedUiState, Core::Api::Interface &core);

     private:
      bool drawWave(const Cairo::RefPtr<Cairo::Context> &ctx);

      SharedState &m_sharedUiState;
      Core::Api::Interface &m_core;
      std::unique_ptr<Core::Api::Computation> m_computation;
    };
  }
}