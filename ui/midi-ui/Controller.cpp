#include "Controller.h"
#include "core/StepWizard.h"
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
    auto tile = m_core.getSelectedTiles().front();

    auto merged = m_core.getMergedPattern();

    merged = Core::processWizard(
        merged,
        static_cast<Core::WizardMode>(std::get<uint8_t>(m_core.getParameter(tile, Core::ParameterId::WizardMode))),
        static_cast<int8_t>(std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardRotate)))),
        static_cast<uint8_t>(std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardOns)))),
        static_cast<uint8_t>(std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardOffs)))));

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

      case Ui::Toolboxes::Steps:
        return buildStepMapping();

      case Ui::Toolboxes::Playground:
        return buildPlaygroundMapping();
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

  Controller::Mapping Controller::buildPlaygroundMapping()
  {
    return {
      .knobs = { { standardBind(Knob::Leftmost, Core::ParameterId::Playground1) },
                 { standardBind(Knob::Rightmost, Core::ParameterId::Playground2) },
                 { standardBind(Knob::NorthWest, Core::ParameterId::Playground3) },
                 { standardBind(Knob::NorthEast, Core::ParameterId::Playground4) },
                 { standardBind(Knob::Center, Core::ParameterId::Playground5) },
                 { standardBind(Knob::SouthWest, Core::ParameterId::Playground6) },
                 { standardBind(Knob::SouthEast, Core::ParameterId::Playground7) } },
      .buttons = {},
    };
  }

  Controller::Mapping Controller::buildStepMapping()
  {
    auto reset = [this]
    {
      auto sel = m_core.getSelectedTiles().front();
      m_core.setParameter(sel, Core::ParameterId::WizardMode, static_cast<uint8_t>(Core::WizardMode::Or));
      m_core.setParameter(sel, Core::ParameterId::WizardRotate, 0.0f);
      m_core.setParameter(sel, Core::ParameterId::WizardOns, 0.0f);
      m_core.setParameter(sel, Core::ParameterId::WizardOffs, 0.0f);
    };

    return { .knobs = { { standardBind(Knob::Rightmost, Core::ParameterId::WizardRotate) },
                        { standardBind(Knob::NorthEast, Core::ParameterId::WizardOns) },
                        { standardBind(Knob::Center, Core::ParameterId::WizardOffs) } },
             .buttonPresses = {
                 { SoftButton::Right_SouthWest,
                   [this]
                   {
                     m_core.setParameter(m_core.getSelectedTiles().front(), Core::ParameterId::WizardMode,
                                         static_cast<uint8_t>(Core::WizardMode::Or));
                   } },

                 { SoftButton::Right_South,
                   [this]
                   {
                     m_core.setParameter(m_core.getSelectedTiles().front(), Core::ParameterId::WizardMode,
                                         static_cast<uint8_t>(Core::WizardMode::And));
                   } },

                 { SoftButton::Right_SouthEast,
                   [this]
                   {
                     m_core.setParameter(m_core.getSelectedTiles().front(), Core::ParameterId::WizardMode,
                                         static_cast<uint8_t>(Core::WizardMode::Replace));
                   } },

                 { SoftButton::Right_East,
                   [this]
                   {
                     m_core.setParameter(m_core.getSelectedTiles().front(), Core::ParameterId::WizardMode,
                                         static_cast<uint8_t>(Core::WizardMode::Not));
                   } },

                 { SoftButton::Right_West, [reset] { reset(); } },

                 { SoftButton::Right_Center,
                   [this, reset]
                   {
                     auto tile = m_core.getSelectedTiles().front();
                     auto p = std::get<Core::Pattern>(m_core.getParameter(tile, Core::ParameterId::Pattern));

                     p = Core::processWizard(
                         p,
                         static_cast<Core::WizardMode>(
                             std::get<uint8_t>(m_core.getParameter(tile, Core::ParameterId::WizardMode))),
                         static_cast<int8_t>(
                             std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardRotate)))),
                         static_cast<uint8_t>(
                             std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardOns)))),
                         static_cast<uint8_t>(
                             std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardOffs)))));

                     m_core.setParameter(tile, Core::ParameterId::Pattern, p);
                     reset();
                   } },
             } };
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
