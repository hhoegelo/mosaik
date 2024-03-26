#include "Controller.h"
#include <core/api/Interface.h>
#include <ui/midi-ui/Interface.h>
#include <dsp/api/display/Interface.h>
#include <ui/touch-ui/Interface.h>

namespace Ui::Midi
{
  static Led stepToLed(Step s)
  {
    return static_cast<Led>(s);
  }

  Controller::Controller(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp, Ui::Touch::Interface &touchUi,
                         Ui::Midi::Interface &midiUi)
      : m_core(core)
      , m_midiUi(midiUi)
      , m_touchUi(touchUi)
      , m_computations(Glib::MainContext::get_default())
  {
    Glib::signal_timeout().connect(
        [this, &dsp, &core]() mutable
        {
          m_currentStep = core.loopPositionToStep(dsp.getCurrentLoopPosition());
          return true;
        },
        16);

    m_computations.add([this] { m_inputMapping = createMapping(m_touchUi.getSelectedToolbox()); });
    m_computations.add([this] { showPattern(); });
  }

  void Controller::showPattern()
  {
    auto merged = m_core.getMergedPattern();

    for(size_t i = 0; i < 64; i++)
    {
      auto isCurrentStep = m_currentStep == i;
      auto isProgrammed = merged[i];
      setLed(stepToLed(i), isCurrentStep ? Color::White : isProgrammed ? Color::Green : Color::Off);
    }
  }

  void Controller::onErpInc(Knob k, int inc)
  {
    if(auto it = m_inputMapping.knobs.find(k); it != m_inputMapping.knobs.end())
      it->second(inc);
  }

  void Controller::onSoftButtonEvent(SoftButton b, ButtonEvent e)
  {
    if(auto it = m_inputMapping.buttons.find(b); it != m_inputMapping.buttons.end())
      it->second(e);
  }

  Controller::Mapping Controller::createMapping(Ui::Toolboxes t)
  {
    switch(t)
    {
      case Ui::Toolboxes::Global:
        return buildGlobalMapping();

      case Ui::Toolboxes::Tile:
        return buildTileMapping();

      case Ui::Toolboxes::Waveform:
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
      = {
          { Knob::Center, [this](auto inc) { m_core.incSelectedTilesParameter(Core::ParameterId::Gain, inc); } },
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
          { Knob::Center, [this](auto inc) { m_core.incSelectedTilesParameter(Core::ParameterId::TriggerFrame, inc * m_touchUi
                                                                                          .getWaveformFramesPerPixel()); } },
          { Knob::Leftmost, [this](auto inc) { m_touchUi.incWaveformZoom(inc); } },
          { Knob::Rightmost, [this](auto inc) { m_touchUi.incWaveformScroll(inc); } },
          { Knob::SouthWest, [this](auto inc) { m_core.incSelectedTilesParameter(Core::ParameterId::EnvelopeFadeInPos, inc * m_touchUi
                                               .getWaveformFramesPerPixel()); } },
          { Knob::NorthWest
            , [this](auto inc) { m_core.incSelectedTilesParameter(Core::ParameterId::EnvelopeFadedInPos, inc * m_touchUi
                                                                                               .getWaveformFramesPerPixel()); } },
          { Knob::SouthEast, [this](auto inc) { m_core.incSelectedTilesParameter(Core::ParameterId::EnvelopeFadeOutPos, inc * m_touchUi
                                                                                                .getWaveformFramesPerPixel()); } },
          { Knob::NorthEast, [this](auto inc) { m_core.incSelectedTilesParameter(Core::ParameterId::EnvelopeFadedOutPos, inc * m_touchUi
                                                                                                .getWaveformFramesPerPixel()); } },


      },
      .buttons = {},
    };
  }

  void Controller::onStepButtonEvent(Step b, ButtonEvent e)
  {
    if(e == ButtonEvent::Press)
    {
      auto merged = m_core.getMergedPattern();
      auto state = merged[b];
      m_core.setStep(b, !state);
    }
  }

  void Controller::setLed(Led led, Color color)
  {
    auto idx = static_cast<size_t>(led);

    if(std::exchange(m_ledLatch[idx], color) != color)
    {
      m_midiUi.setLed(led, color);
    }
  }
}
