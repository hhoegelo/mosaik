#include <memory>
#include "Ui.h"
#include <midi/Monitor.h>

namespace Ui
{
  namespace Midi
  {
    Ui::Ui(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp)
        : m_monitor(std::make_unique<::Midi::Monitor>())
    {
    }

    Ui::~Ui() = default;
  }
}
