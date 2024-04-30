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
  {
    Glib::signal_timeout().connect(
        [this, &dsp, &core]() mutable
        {
          m_currentStep = core.loopPositionToStep(dsp.getCurrentLoopPosition());
          return true;
        },
        16);

    m_computations.add([this] { m_inputMapping = createMapping(m_touchUi.getToolboxes().getSelectedToolbox()); });
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

  template <Ui::Toolbox T, Core::ParameterId ID>
  std::pair<Knob, std::function<void(int)>> Controller::bindKnobRotation()
  {
    std::pair<Knob, std::function<void(int)>> ret;

    using E = ToolboxDefinition<T>::MaximizedParameters;
    E::forEach(
        [&](auto a)
        {
          if(a.id == ID)
          {
            ret = std::make_pair(std::get<Knob>(a.position),
                                 [this](int inc)
                                 {
                                   auto tile = Core::GlobalParameters<Core::NoWrap>::contains(ID)
                                       ? Core::TileId {}
                                       : m_core.getSelectedTile();
                                   m_core.incParameter(tile, ID, inc);
                                 });
          }
        });

    return ret;
  };

  template <Ui::Toolbox T, Core::ParameterId ID>
  std::pair<SoftButton, std::function<void()>> Controller::bindButtonToggle()
  {
    std::pair<SoftButton, std::function<void()>> ret;

    using E = ToolboxDefinition<T>::MaximizedParameters;
    E::forEach(
        [&](auto a)
        {
          if(a.id == ID)
            ret = std::make_pair(std::get<SoftButton>(a.position), [this] { m_core.toggleSelectedTilesParameter(ID); });
        });

    return ret;
  }

  std::pair<Knob, std::function<void(int)>> Controller::standardZoomedBind(Knob k, Core::ParameterId p)
  {
    auto r = [this, p](int inc)
    {
      auto tile
          = Core::GlobalParameters<Core::NoWrap>::contains(p) ? Core::TileId {} : m_core.getSelectedTiles().front();
      auto fpp = m_touchUi.getToolboxes().getWaveform().getFramesPerPixel();
      m_core.incParameter(tile, p, inc * fpp);
    };
    return std::make_pair(k, r);
  };

  Controller::Mapping Controller::createMapping(Ui::Toolbox t)
  {
    switch(t)
    {
      case Ui::Toolbox::Global:
        return buildMapping<Toolbox::Global>();

      case Ui::Toolbox::Tile:
        return buildTileMapping();

      case Ui::Toolbox::Waveform:
        return buildWaveformMapping();

      case Ui::Toolbox::Steps:
        return buildStepMapping();

      case Ui::Toolbox::Playground:
        return buildMapping<Toolbox::Playground>();

      case Ui::Toolbox::MainPlayground:
        return buildMapping<Toolbox::MainPlayground>();
    }
    throw std::runtime_error("unknown toolbox");
  }

  template <Toolbox T> Controller::Mapping Controller::buildMapping()
  {
    Controller::Mapping mapping;

    ToolboxDefinition<T>::MaximizedParameters::forEach(
        [&](auto a)
        {
          using D = decltype(a);
          switch(D::binding)
          {
            case UIBinding::ReleasedKnobRotate:
              mapping.knobs.insert(bindKnobRotation<T, D::id>());
              break;

            case UIBinding::ButtonPressToggle:
              mapping.buttonPresses.insert(bindButtonToggle<T, D::id>());
              break;

            case UIBinding::ButtonReleaseToggle:
              mapping.buttonReleases.insert(bindButtonToggle<T, D::id>());
              break;

            default:
              throw std::runtime_error("Unsupported ui binding");
          }
        });

    return mapping;
  }

  template <typename I> struct Bind
  {
    using ID = I;
    std::function<void()> handler;
  };

  template <Ui::Toolbox T, typename... Bindings>
  void Controller::addCustomInvokations(Mapping &mapping, Bindings... bindings)
  {
    ToolboxDefinition<T>::MaximizedCustom::forEach(
        [&](auto a)
        {
          using MaximizedUiOnly = decltype(a);

          auto insertIfIdMatches = [&](std::map<SoftButton, std::function<void()>> &target, auto b)
          {
            using Binding = decltype(b);
            if constexpr(std::is_same_v<typename Binding::ID, typename MaximizedUiOnly::ID>)
            {
              target[std::get<SoftButton>(MaximizedUiOnly::position)] = b.handler;
            }
          };

          switch(MaximizedUiOnly::binding)
          {
            case UIBinding::ButtonPressInvoke:
              (insertIfIdMatches(mapping.buttonPresses, bindings), ...);
              break;

            default:
              throw std::runtime_error("Unsupported ui binding");
          }
        });
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

    auto apply = [this, reset]
    {
      auto tile = m_core.getSelectedTiles().front();
      auto p = std::get<Core::Pattern>(m_core.getParameter(tile, Core::ParameterId::Pattern));

      p = Core::processWizard(
          p, static_cast<Core::WizardMode>(std::get<uint8_t>(m_core.getParameter(tile, Core::ParameterId::WizardMode))),
          static_cast<int8_t>(std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardRotate)))),
          static_cast<uint8_t>(std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardOns)))),
          static_cast<uint8_t>(std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardOffs)))));

      m_core.setParameter(tile, Core::ParameterId::Pattern, p);
      reset();
    };

    auto setMode = [&](Core::WizardMode mode)
    { m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::WizardMode, static_cast<uint8_t>(mode)); };

    auto mapping = buildMapping<Toolbox::Steps>();
    addCustomInvokations<Toolbox::Steps>(
        mapping, Bind<ToolboxDefinition<Toolbox::Steps>::Apply>(apply),
        Bind<ToolboxDefinition<Toolbox::Steps>::Cancel>(reset),
        Bind<ToolboxDefinition<Toolbox::Steps>::And>([setMode] { setMode(Core::WizardMode::And); }),
        Bind<ToolboxDefinition<Toolbox::Steps>::Or>([setMode] { setMode(Core::WizardMode::Or); }),
        Bind<ToolboxDefinition<Toolbox::Steps>::Not>([setMode] { setMode(Core::WizardMode::Not); }));

    return mapping;
  }

  Controller::Mapping Controller::buildTileMapping()
  {
    Controller::Mapping mapping = buildMapping<Toolbox::Tile>();

    addCustomInvokations<Toolbox::Tile>(
        mapping,
        Bind<ToolboxDefinition<Toolbox::Tile>::Up>([this] { m_touchUi.getToolboxes().getFileBrowser().dec(); }),
        Bind<ToolboxDefinition<Toolbox::Tile>::Down>([this] { m_touchUi.getToolboxes().getFileBrowser().inc(); }),
        Bind<ToolboxDefinition<Toolbox::Tile>::Enter>([this] { m_touchUi.getToolboxes().getFileBrowser().down(); }),
        Bind<ToolboxDefinition<Toolbox::Tile>::Leave>([this] { m_touchUi.getToolboxes().getFileBrowser().up(); }),
        Bind<ToolboxDefinition<Toolbox::Tile>::Prelisten>([this]
                                                          { m_touchUi.getToolboxes().getFileBrowser().prelisten(); }),
        Bind<ToolboxDefinition<Toolbox::Tile>::Load>([this] { m_touchUi.getToolboxes().getFileBrowser().load(); }));

    return mapping;
  }

  Controller::Mapping Controller::buildWaveformMapping()
  {
    return {
      .knobs = { { Knob::Center,
                   [this](auto inc)
                   {
                     auto sel = m_core.getSelectedTiles().front();
                     auto reverse = std::get<bool>(m_core.getParameter(sel, Core::ParameterId::Reverse));
                     auto fpp = m_touchUi.getToolboxes().getWaveform().getFramesPerPixel();
                     m_core.incParameter(sel, Core::ParameterId::TriggerFrame, reverse ? -fpp * inc : fpp * inc);
                   } },
                 { Knob::Leftmost, [this](auto inc) { m_touchUi.getToolboxes().getWaveform().incZoom(inc); } },
                 { Knob::Rightmost, [this](auto inc) { m_touchUi.getToolboxes().getWaveform().incScroll(inc); } },
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
