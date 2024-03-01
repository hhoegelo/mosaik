#include "Controller.h"
#include <core/api/Interface.h>
#include <ui/midi-ui/Interface.h>
#include <dsp/api/display/Interface.h>

namespace Ui::Midi
{
  Controller::Controller(SharedState &sharedUiState, Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp,
                         Ui::Midi::Interface &ui)
      : m_core(core)
      , m_ui(ui)
  {
    Glib::signal_timeout().connect(
        [&ui, &dsp, lastStep = -1]() mutable
        {
          auto newStep = dsp.getCurrentStep();

          if(newStep != lastStep)
          {
            ui.highlightCurrentStep(lastStep, newStep);
            lastStep = newStep;
          }

          return true;
        },
        16);
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