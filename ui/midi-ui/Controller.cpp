#include "Controller.h"
#include <core/api/Interface.h>
#include <ui/midi-ui/Interface.h>
#include <dsp/api/display/Interface.h>

namespace Ui::Midi
{
  Controller::Controller(SharedState &sharedUiState, Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp,
                         Ui::Midi::Interface &ui)
      : m_sharedUiState(sharedUiState)
      , m_core(core)
      , m_ui(ui)
      , m_computations(Glib::MainContext::get_default())
  {
    Glib::signal_timeout().connect(
        [&ui, &dsp, &core, lastStep = -1]() mutable
        {
          auto loopPosition = dsp.getCurrentLoopPosition();
          auto newStep = core.loopPositionToStep(loopPosition);

          if(newStep != lastStep)
          {
            ui.highlightCurrentStep(lastStep, newStep);
            lastStep = newStep;
          }

          return true;
        },
        16);

    m_computations.add([this] { m_inputMapping = createMapping(m_sharedUiState.getSelectedToolbox()); });
  }

  void Controller::kickOff()
  {
    m_computations.add([this] { showPattern(); });
  }

  void Controller::showPattern()
  {
    auto merged = m_core.getMergedPattern();

    for(size_t i = 0; i < 64; i++)
      m_ui.setStepButtonColor(i, merged[i] ? Ui::Midi::Color::Green : Ui::Midi::Color::White);
  }

  void Controller::onErpInc(Knob k, int inc)
  {
    if(auto it = m_inputMapping.knobs.find(k); it != m_inputMapping.knobs.end())
      it->second(inc);
  }

  void Controller::onButtonEvent(SoftButton b, ButtonEvent e)
  {
    if(auto it = m_inputMapping.buttons.find(b); it != m_inputMapping.buttons.end())
      it->second(e);
  }

  Controller::Mapping Controller::createMapping(Ui::SharedState::Toolboxes t)
  {
    switch(t)
    {
      case SharedState::Toolboxes::Global:
        return buildGlobalMapping();

      case SharedState::Toolboxes::Tile:
        return buildTileMapping();

      case SharedState::Toolboxes::Waveform:
        return buildWaveformMapping();
    }
    throw std::runtime_error("unknown toolbox");
  }

  Controller::Mapping Controller::buildGlobalMapping()
  {
    return {
      .knobs
      = { { Knob::Center, [this](auto inc) { m_core.incParameter({}, Core::ParameterId::GlobalVolume, inc); } },
          { Knob::NorthEast, [this](auto inc) { m_core.incParameter({}, Core::ParameterId::GlobalShuffle, inc); } },
          { Knob::SouthEast, [this](auto inc) { m_core.incParameter({}, Core::ParameterId::GlobalTempo, inc); } } },
      .buttons = {},
    };
  }

  Controller::Mapping Controller::buildTileMapping()
  {
    return {
      .knobs
      = { { Knob::Center, [this](auto inc) { m_core.incSelectedTilesParameter(Core::ParameterId::Gain, inc); } },
          { Knob::NorthWest, [this](auto inc) { m_core.incSelectedTilesParameter(Core::ParameterId::EnvelopeFadeInPos, inc); } },
          { Knob::NorthEast
            , [this](auto inc) { m_core.incSelectedTilesParameter(Core::ParameterId::EnvelopeFadeInLen, inc); } },
          { Knob::SouthWest, [this](auto inc) { m_core.incSelectedTilesParameter(Core::ParameterId::EnvelopeFadeOutPos, inc); } },
          { Knob::SouthEast, [this](auto inc) { m_core.incSelectedTilesParameter(Core::ParameterId::EnvelopeFadeOutLen, inc); } },
          { Knob::Rightmost, [this](auto inc) { m_core.incSelectedTilesParameter(Core::ParameterId::Speed, inc); } },
          { Knob::Leftmost, [this](auto inc) { m_core.incSelectedTilesParameter(Core::ParameterId::Balance, inc); } },
           },
      .buttons = {
          { SoftButton::Right_Center,
            [this](auto e) {
              if(e == ButtonEvent::Release)
              {m_core.toggleSelectedTilesParameter(Core::ParameterId::Reverse);} } }
      },
    };
  }

  Controller::Mapping Controller::buildWaveformMapping()
  {
    return {
      .knobs = {
          { Knob::NorthWest, [this](auto inc) { m_sharedUiState.incWaveformZoom(inc); } },
          { Knob::NorthEast, [this](auto inc) { m_sharedUiState.incWaveformScroll(inc); } },

      },
      .buttons = {},
    };
  }

}