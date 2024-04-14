#include "Controller.h"
#include <ui/Types.h>
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

    if(e == ButtonEvent::Press)
      if(auto it = m_inputMapping.buttonPresses.find(b); it != m_inputMapping.buttonPresses.end())
        it->second();

    if(e == ButtonEvent::Release)
      if(auto it = m_inputMapping.buttonReleases.find(b); it != m_inputMapping.buttonReleases.end())
        it->second();
  }

  std::pair<Knob, std::function<void(int)>> Controller::standardBind(Knob k, Core::ParameterId p)
  {
    auto r = [this, p](int inc)
    {
      auto tile = Core::GlobalParameters ::contains(p) ? Core::TileId {} : m_core.getSelectedTiles().front();
      m_core.incParameter(tile, p, inc);
    };
    return std::make_pair(k, r);
  };

  std::pair<SoftButton, std::function<void()>> Controller::standardBindRelease(SoftButton k, Core::ParameterId p)
  {
    auto r = [this, p] { m_core.toggleSelectedTilesParameter(p); };
    return std::make_pair(k, r);
  }

  std::pair<Knob, std::function<void(int)>> Controller::standardZoomedBind(Knob k, Core::ParameterId p)
  {
    auto r = [this, p](int inc)
    {
      auto tile = Core::GlobalParameters::contains(p) ? Core::TileId {} : m_core.getSelectedTiles().front();
      auto fpp = m_touchUi.getWaveform().getFramesPerPixel();
      m_core.incParameter(tile, p, inc * fpp);
    };
    return std::make_pair(k, r);
  };

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
      .knobs = { { standardBind(Knob::Center, Core::ParameterId::GlobalVolume) },
                 { standardBind(Knob::SouthEast, Core::ParameterId::GlobalTempo) } },
      .buttons = {},
    };
  }

  Controller::Mapping Controller::buildTileMapping()
  {
    return {
      .knobs
      = {
          { standardBind(Knob::Center,Core::ParameterId::Gain) },
          { standardBind(Knob::Rightmost,Core::ParameterId::Speed) },
          { standardBind(Knob::Leftmost,Core::ParameterId::Balance)},
          { standardBind(Knob::SouthWest,Core::ParameterId::Shuffle)},
           },
      .buttonReleases = {
          { standardBindRelease(SoftButton::Left_Center,Core::ParameterId::Reverse)},
          { SoftButton::Right_North, [this] {
      m_touchUi.getFileBrowser().dec(); } },
          { SoftButton::Right_Center, [this] {
      m_touchUi.getFileBrowser().load(); } },
          { SoftButton::Right_South, [this] {
      m_touchUi.getFileBrowser().inc(); } },
          { SoftButton::Right_West, [this] {
      m_touchUi.getFileBrowser().up(); } },
          { SoftButton::Right_East, [this] {
      m_touchUi.getFileBrowser().down(); } },
          { SoftButton::Right_NorthEast, [this] {
              m_touchUi.getFileBrowser().prelisten(); } },
      },
    };
  }

  Controller::Mapping Controller::buildWaveformMapping()
  {
    return {
      .knobs = { { standardZoomedBind(Knob::Center, Core::ParameterId::TriggerFrame) },
                 { Knob::Leftmost, [this](auto inc) { m_touchUi.getWaveform().incZoom(inc); } },
                 { Knob::Rightmost, [this](auto inc) { m_touchUi.getWaveform().incScroll(inc); } },
                 { standardZoomedBind(Knob::SouthWest, Core::ParameterId::EnvelopeFadeInPos) },
                 { standardZoomedBind(Knob::NorthWest, Core::ParameterId::EnvelopeFadedInPos) },
                 { standardZoomedBind(Knob::NorthEast, Core::ParameterId::EnvelopeFadeOutPos) },
                 { standardZoomedBind(Knob::SouthEast, Core::ParameterId::EnvelopeFadedOutPos) } },
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
