#include "Controller.h"
#include <core/api/Interface.h>
#include <ui/midi-ui/Interface.h>
#include <ranges>

namespace Ui::Midi
{
  Controller::Controller(SharedState &sharedUiState, Core::Api::Interface &core, Ui::Midi::Interface &ui)
      : m_core(core)
      , m_ui(ui)
  {
  }

  void Controller::kickOff()
  {
    showPattern();
  }

  void Controller::showPattern()
  {
    m_pattern = std::make_unique<Core::Api::Computation>();
    auto merged = m_core.getMergedPattern(m_pattern.get());

    for(size_t i = 0; i < 64; i++)
      m_ui.setStepButtonColor(i, merged[i] ? Ui::Midi::Color::Green : Ui::Midi::Color::White);

    m_pattern->refresh([this] { showPattern(); });
  }

}