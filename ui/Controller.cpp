#include "Controller.h"
#include "core/ParameterDescriptor.h"
#include "Types.h"
#include "core/api/Interface.h"
#include "ui/midi-ui/Interface.h"
#include "dsp/api/display/Interface.h"
#include "ui/touch-ui/Interface.h"
#include "ToolboxDefinition.h"
#include <cinttypes>
#include <cmath>

#if(__GNUC__ > 12)
#define UNSUPPORTED_BRANCH() static_assert(false)
#else
#define UNSUPPORTED_BRANCH() throw std::runtime_error("unsupported branch")
#endif

using namespace std::chrono_literals;

namespace Ui
{
  static Led stepToLed(Step s)
  {
    return static_cast<Led>(s);
  }

  Controller::Controller(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp)
      : m_core(core)
  {
    Glib::signal_timeout().connect(
        [this, &dsp, &core]() mutable
        {
          m_currentStep = core.loopPositionToStep(dsp.getCurrentLoopPosition());
          return true;
        },
        16);
  }

  void Controller::init(Touch::Interface &touchUi)
  {
    m_touchUi = &touchUi;
  }

  void Controller::run()
  {
    m_computations.add(
        [this]
        {
          if(auto p = m_touchUi.get())
            m_inputMapping = createMapping(p->getToolboxes().getSelectedToolbox());
        });
    m_computations.add([this] { showPattern(); });
  }

  void Controller::addUi(Midi::Interface &midiUI)
  {
    m_midiUi.push_back(&midiUI);
  }

  void Controller::showPattern()
  {
    auto tile = m_core.getSelectedTile();
    auto pattern = std::get<Core::Pattern>(m_core.getParameter(tile, Core::ParameterId::Pattern));

    for(size_t i = 0; i < 64; i++)
    {
      auto isCurrentStep = m_currentStep == i;
      auto isProgrammed = pattern[i];
      setLed(stepToLed(i), isCurrentStep ? Color::White : isProgrammed ? Color::Green : Color::Off);
    }
  }

  void Controller::onErpInc(Knob k, int inc)
  {
    m_turnWhilePressed[static_cast<size_t>(k)] = true;

    if(m_buttonState[static_cast<int>(getButtonForKnob(k))])
    {
      if(auto it = m_inputMapping.knobIncDecPressed.find(k); it != m_inputMapping.knobIncDecPressed.end())
        it->second(inc);
    }
    else
    {
      if(auto it = m_inputMapping.knobIncDecReleased.find(k); it != m_inputMapping.knobIncDecReleased.end())
        it->second(inc);
    }
  }

  void Controller::onSoftButtonEvent(SoftButton b, ButtonEvent e)
  {
    m_buttonState[static_cast<int>(b)] = (e == ButtonEvent::Press);

    if(e == ButtonEvent::Press)
    {
      if(isKnob(b))
      {
        auto k = getKnobForButton(b);
        m_turnWhilePressed[static_cast<size_t>(k)] = false;
      }

      if(auto it = m_inputMapping.buttonPressed.find(b); it != m_inputMapping.buttonPressed.end())
        it->second();
    }

    if(e == ButtonEvent::Release)
    {
      if(auto it = m_inputMapping.buttonReleased.find(b); it != m_inputMapping.buttonReleased.end())
        it->second();

      if(isKnob(b))
      {
        auto k = getKnobForButton(b);
        if(!m_turnWhilePressed[static_cast<size_t>(k)])
        {
          if(auto it = m_inputMapping.knobClick.find(k); it != m_inputMapping.knobClick.end())
            it->second();
        }
      }
    }
  }

  Controller::Mapping Controller::createMapping(Ui::Toolbox t)
  {
    switch(t)
    {
      case Ui::Toolbox::Global:
        return buildMapping<Toolbox::Global>();

      case Ui::Toolbox::Tile:
        return buildMapping<Toolbox::Tile>();

      case Ui::Toolbox::Waveform:
        return buildMapping<Toolbox::Waveform>();

      case Ui::Toolbox::Steps:
        return buildMapping<Toolbox::Steps>();

      case Ui::Toolbox::Playground:
        return buildMapping<Toolbox::Playground>();

      case Ui::Toolbox::MainPlayground:
        return buildMapping<Toolbox::MainPlayground>();

      case Ui::Toolbox::Mute:
        return buildMapping<Toolbox::Mute>();
    }
    throw std::runtime_error("unknown toolbox");
  }

  template <Toolbox T, typename D>
  std::pair<Knob, std::function<void(int)>> Controller::bindKnobUiParameterAction(float factor)
  {
    constexpr bool isGlobal = Core::GlobalParameters<Core::NoWrap>::contains(D::id);
    if constexpr(D::action == UiAction::IncDec)
      return std::make_pair(std::get<Knob>(D::position),
                            [this, factor](int inc)
                            {
                              auto tile = isGlobal ? Core::TileId {} : m_core.getSelectedTile();
                              m_core.incParameter(tile, D::id, factor * inc);
                            });
    else if constexpr(D::action == UiAction::IncDecZoomed)
      return std::make_pair(std::get<Knob>(D::position),
                            [this, factor](int inc)
                            {
                              if(auto p = m_touchUi.get())
                              {
                                auto tile = isGlobal ? Core::TileId {} : m_core.getSelectedTile();
                                auto fpp = p->getToolboxes().getWaveform().getFramesPerPixel();
                                m_core.incParameter(tile, D::id, factor * fpp * inc);
                              }
                            });
    else
      UNSUPPORTED_BRANCH();
  }

  template <Toolbox T, typename D>
  std::pair<SoftButton, std::function<void()>> Controller::bindButtonUiParameterAction()
  {
    if constexpr(D::action == UiAction::Toggle)
      return std::make_pair(std::get<SoftButton>(D::position),
                            [this]() { m_core.toggleSelectedTilesParameter(D::id); });
    else
      UNSUPPORTED_BRANCH();
  }

  template <Toolbox T, typename D> std::pair<Knob, std::function<void()>> Controller::bindKnobUiClickAction()
  {
    if constexpr(D::action == UiAction::Default)
      return bindKnobUiDefaultClickAction<T, D>();
    else
      UNSUPPORTED_BRANCH();
  }

  template <Toolbox T, typename D> std::pair<Knob, std::function<void()>> Controller::bindKnobUiDefaultClickAction()
  {
    constexpr bool isGlobal = Core::GlobalParameters<Core::NoWrap>::contains(D::id);
    auto tile = isGlobal ? Core::TileId {} : m_core.getSelectedTile();
    return std::make_pair(std::get<Knob>(D::position), [this, tile]()
                          { m_core.setParameter(tile, D::id, ParameterDescriptor<D::id>::defaultValue); });
  }

  template <Toolbox T, typename D> std::pair<SoftButton, std::function<void()>> Controller::bindButtonUiInvokeAction()
  {
    if constexpr(D::action == UiAction::Invoke)
      return std::make_pair(std::get<SoftButton>(D::position),
                            [this]() { this->invokeButtonAction<T, typename D::ID>(); });
    else
      UNSUPPORTED_BRANCH();
  }

  template <Toolbox T, typename D> std::pair<Knob, std::function<void(int)>> Controller::bindKnobUiInvokeAction()
  {
    if constexpr(D::action == UiAction::Invoke)
      return std::make_pair(std::get<Knob>(D::position),
                            [this](int i) { this->invokeKnobAction<T, typename D::ID>(i); });
    else
      UNSUPPORTED_BRANCH();
  }

  template <Toolbox T> Controller::Mapping Controller::buildMapping()
  {
    Controller::Mapping mapping;

    ToolboxDefinition<T>::MaximizedParameters::forEach(
        [&](auto a)
        {
          using D = decltype(a);
          if constexpr(D::event == UiEvent::ReleasedKnobRotate)
          {
            mapping.knobIncDecReleased.insert(bindKnobUiParameterAction<T, D>());

            using P = ParameterDescriptor<D::id>;

            if constexpr(requires(P) { P::defaultValue; })
            {
              mapping.knobClick.insert(bindKnobUiDefaultClickAction<T, D>());
            }

            if constexpr(requires(P) { P::acceleration; })
            {
              mapping.knobIncDecPressed.insert(bindKnobUiParameterAction<T, D>(P::acceleration));
            }
          }
          else if constexpr(D::event == UiEvent::PressedKnobRotate)
            mapping.knobIncDecPressed.insert(bindKnobUiParameterAction<T, D>());
          else if constexpr(D::event == UiEvent::ButtonPress)
            mapping.buttonPressed.insert(bindButtonUiParameterAction<T, D>());
          else if constexpr(D::event == UiEvent::ButtonRelease)
            mapping.buttonReleased.insert(bindButtonUiParameterAction<T, D>());
          else if constexpr(D::event == UiEvent::KnobClick)
            mapping.knobClick.insert(bindKnobUiClickAction<T, D>());
          else
            throw std::runtime_error("Unsupported ui binding");
        });

    ToolboxDefinition<T>::MaximizedCustom::forEach(
        [&](auto a)
        {
          using D = decltype(a);

          if constexpr(D::event == UiEvent::ButtonPress)
            mapping.buttonPressed.insert(bindButtonUiInvokeAction<T, D>());
          else if constexpr(D::event == UiEvent::ButtonRelease)
            mapping.buttonReleased.insert(bindButtonUiInvokeAction<T, D>());
          else if constexpr(D::event == UiEvent::ReleasedKnobRotate)
            mapping.knobIncDecReleased.insert(bindKnobUiInvokeAction<T, D>());
          else if constexpr(D::event == UiEvent::PressedKnobRotate)
            mapping.knobIncDecPressed.insert(bindKnobUiInvokeAction<T, D>());
          else
            UNSUPPORTED_BRANCH();
        });

    return mapping;
  }

  template <> void Controller::invokeButtonAction<Toolbox::Global, ToolboxDefinition<Toolbox::Global>::TapNSync>()
  {
    m_core.addTap();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Up>()
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getFileBrowser().dec();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Down>()
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getFileBrowser().inc();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Leave>()
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getFileBrowser().up();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Enter>()
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getFileBrowser().down();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Prelisten>()
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getFileBrowser().prelisten();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Load>()
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getFileBrowser().load();
  }

  template <> void Controller::invokeKnobAction<Toolbox::Waveform, ToolboxDefinition<Toolbox::Waveform>::Zoom>(int inc)
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getWaveform().incZoom(inc);
  }

  template <>
  void Controller::invokeKnobAction<Toolbox::Waveform, ToolboxDefinition<Toolbox::Waveform>::Scroll>(int inc)
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getWaveform().incScroll(inc);
  }

  template <>
  void Controller::invokeKnobAction<Toolbox::Waveform, ToolboxDefinition<Toolbox::Waveform>::HitPoint>(int inc)
  {
    if(auto p = m_touchUi.get())
    {
      auto sel = m_core.getSelectedTile();
      auto reverse = std::get<bool>(m_core.getParameter(sel, Core::ParameterId::Reverse));
      auto fpp = p->getToolboxes().getWaveform().getFramesPerPixel();
      m_core.incParameter(sel, Core::ParameterId::TriggerFrame, reverse ? -fpp * inc : fpp * inc);
    }
  }

  template <> void Controller::invokeKnobAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::OneFitsAll>(int inc)
  {
    m_oneFitsAllStepWizard = std::clamp(m_oneFitsAllStepWizard + inc, 0, 255);

    auto sel = m_core.getSelectedTile();
    Core::Pattern pattern {};

    int64_t w = std::abs(m_oneFitsAllStepWizard);

    if(w <= 0x0F)
      w = w | w << 4;

    if(w <= 0xFF)
      w = w | w << 8;

    if(w <= 0xFFFF)
      w = w | w << 16;

    if(w <= 0xFFFFFFFF)
      w = w | w << 32;

    uint64_t m = 1;

    for(size_t i = 0; i < NUM_STEPS; i++)
      pattern[i] = w & (m << i);

    m_core.setParameter(sel, Core::ParameterId::Pattern, pattern);
  }

  template <> void Controller::invokeKnobAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::Rotate>(int inc)
  {
    auto sel = m_core.getSelectedTile();
    auto pattern = std::get<Core::Pattern>(m_core.getParameter(sel, Core::ParameterId::Pattern));

    if(inc > 0)
      std::rotate(pattern.rbegin(), pattern.rbegin() + std::abs(inc), pattern.rend());
    else if(inc < 0)
      std::rotate(pattern.begin(), pattern.begin() + std::abs(inc), pattern.end());

    m_wizardRotation = m_wizardRotation + inc;

    m_core.setParameter(sel, Core::ParameterId::Pattern, pattern);
  }

  template <> void Controller::invokeKnobAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::Steps>(int inc)
  {
    m_wizardSteps = std::clamp(m_wizardSteps + inc, 0, 64);
    processStepsGapsWizard();
  }

  template <> void Controller::invokeKnobAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::Gaps>(int inc)
  {
    m_wizardGaps = std::clamp(m_wizardGaps + inc, 0, 64);
    processStepsGapsWizard();
  }

  void Controller::processStepsGapsWizard()
  {
    Core::Pattern pattern = {};

    if(m_wizardSteps || m_wizardGaps)
    {
      for(int i = 0; i < NUM_STEPS;)
      {
        for(int x = 0; x < m_wizardSteps; x++)
        {
          if(i < NUM_STEPS)
            pattern[i] = true;

          i++;
        }

        i += m_wizardGaps;
      }

      if(m_wizardRotation > 0)
        std::rotate(pattern.rbegin(), pattern.rbegin() + std::abs(m_wizardRotation), pattern.rend());
      else if(m_wizardRotation < 0)
        std::rotate(pattern.begin(), pattern.begin() + std::abs(m_wizardRotation), pattern.end());

      if(m_wizardInvert.get())
      {
        for(auto &a : pattern)
          a = !a;
      }

      m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern, pattern);
    }
  }

  template <> void Controller::invokeButtonAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::All>()
  {
    Core::Pattern pattern = {};
    pattern.fill(true);
    m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern, pattern);
  }

  template <> void Controller::invokeButtonAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::None>()
  {
    Core::Pattern pattern = {};
    pattern.fill(false);
    m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern, pattern);
  }

  template <> void Controller::invokeButtonAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::Invert>()
  {
    m_wizardInvert = !m_wizardInvert.get();
    auto pattern = std::get<Core::Pattern>(m_core.getParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern));
    for(auto &a : pattern)
      a = !a;
    m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern, pattern);
  }

  template <> void Controller::invokeButtonAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::Mirror>()
  {
    m_wizardMirror = !m_wizardMirror.get();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Mute, ToolboxDefinition<Toolbox::Mute>::LastMute>()
  {
    for(uint8_t i = 0; i < NUM_TILES; i++)
    {
      m_core.setParameter(i, Core::ParameterId::Mute, m_savedMute[i]);
    }
  }

  template <> void Controller::invokeButtonAction<Toolbox::Mute, ToolboxDefinition<Toolbox::Mute>::SaveArmed>()
  {
    m_saveArmed = true;
  }

  template <> void Controller::invokeButtonAction<Toolbox::Mute, ToolboxDefinition<Toolbox::Mute>::SaveUnarmed>()
  {
    m_saveArmed = false;
  }

  template <> void Controller::invokeButtonAction<Toolbox::Mute, ToolboxDefinition<Toolbox::Mute>::Slot1>()
  {
    handleMuteSlot(0);
  }

  template <> void Controller::invokeButtonAction<Toolbox::Mute, ToolboxDefinition<Toolbox::Mute>::Slot2>()
  {
    handleMuteSlot(1);
  }

  template <> void Controller::invokeButtonAction<Toolbox::Mute, ToolboxDefinition<Toolbox::Mute>::Slot3>()
  {
    handleMuteSlot(2);
  }

  template <> void Controller::invokeButtonAction<Toolbox::Mute, ToolboxDefinition<Toolbox::Mute>::Slot4>()
  {
    handleMuteSlot(3);
  }

  template <> void Controller::invokeButtonAction<Toolbox::Mute, ToolboxDefinition<Toolbox::Mute>::Slot5>()
  {
    handleMuteSlot(4);
  }

  template <> void Controller::invokeButtonAction<Toolbox::Mute, ToolboxDefinition<Toolbox::Mute>::Slot6>()
  {
    handleMuteSlot(5);
  }

  template <> void Controller::invokeButtonAction<Toolbox::Mute, ToolboxDefinition<Toolbox::Mute>::UnmuteAll>()
  {
    for(uint8_t i = 0; i < NUM_TILES; i++)
    {
      m_savedMute[i] = std::get<bool>(m_core.getParameter(i, Core::ParameterId::Mute));
      m_core.setParameter(i, Core::ParameterId::Mute, false);
    }
  }

  void Controller::onStepButtonEvent(Step b, ButtonEvent e)
  {
    if(e == ButtonEvent::Press)
    {
      auto pattern = std::get<Core::Pattern>(m_core.getParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern));
      auto state = pattern[b];
      auto newState = !state;
      auto idx = b;
      pattern[idx] = newState;

      if(m_wizardMirror.get())
      {
        idx = idx % 16;
        pattern[idx] = newState;
        pattern[idx + 16] = newState;
        pattern[idx + 32] = newState;
        pattern[idx + 48] = newState;
      }
      m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern, pattern);
    }
  }

  void Controller::setLed(Led led, Color color)
  {
    auto idx = static_cast<size_t>(led);

    if(std::exchange(m_ledLatch[idx], color) != color)
    {
      for(auto a : m_midiUi)
        a->setLed(led, color);
    }
  }

  /*
   * GET DISPLAY VALUES
   */
  template <Core::ParameterId id> struct WrapParameterDescription
  {
    using Wrapped = ParameterDescriptor<id>;
  };

  using GlobalParameters = Core::GlobalParameters<WrapParameterDescription>::Wrapped;
  using TileParameters = Core::TileParameters<WrapParameterDescription>::Wrapped;

  template <typename Description>
  bool fillString(std::string &target, Core::Api::Interface &core, Core::TileId tile, Core::ParameterId id)
  {
    if(Description::id == id)
    {
      target = Description::format(std::get<typename Description::Type>(core.getParameter(tile, id)));
      return true;
    }

    return false;
  }

  std::string Controller::getDisplayValue(Core::TileId tile, Core::ParameterId id)
  {
    std::string ret;
    GlobalParameters globalParams {};

    if(!std::apply([&](auto... a) { return (fillString<decltype(a)>(ret, m_core, {}, id) || ...); },
                   GlobalParameters {}))
      std::apply([&](auto... a) { return (fillString<decltype(a)>(ret, m_core, tile, id) || ...); }, TileParameters {});

    return ret;
  }

  std::string Controller::getDisplayValue(Core::ParameterId id)
  {
    return getDisplayValue(m_core.getSelectedTile(), id);
  }

  void Controller::handleMuteSlot(int slot)
  {
    if(m_saveArmed.get())
    {
      MuteState s;
      for(uint8_t i = 0; i < NUM_TILES; i++)
        s[i] = std::get<bool>(m_core.getParameter(i, Core::ParameterId::Mute));
      m_savedMutes[slot] = s;
    }
    else
    {
      MuteState s = m_savedMutes[slot];
      for(uint8_t i = 0; i < NUM_TILES; i++)
        m_core.setParameter(i, Core::ParameterId::Mute, s[i]);
    }
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Waveform>::Zoom>()
  {
    if(auto p = m_touchUi.get())
      return Tools::format("x %3.2f", p->getToolboxes().getWaveform().getZoom());
    return "";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Waveform>::Scroll>()
  {
    if(auto p = m_touchUi.get())
      return Tools::format("%" PRId64 " frames", p->getToolboxes().getWaveform().getScroll());
    return "";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Waveform>::HitPoint>()
  {
    return Tools::format(
        "%" PRId64 " frames",
        std::get<Core::FramePos>(m_core.getParameter(m_core.getSelectedTile(), Core::ParameterId::TriggerFrame)));
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Steps>::Gaps>()
  {
    return std::to_string(m_wizardGaps);
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Steps>::Steps>()
  {
    return std::to_string(m_wizardSteps);
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Steps>::Rotate>()
  {
    return std::to_string(m_wizardRotation);
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Steps>::Invert>()
  {
    return m_wizardInvert.get() ? "On" : "Off";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Steps>::Mirror>()
  {
    return m_wizardMirror.get() ? "On" : "Off";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Mute>::SaveArmed>()
  {
    return m_saveArmed.get() ? "Armed" : "";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Mute>::SaveUnarmed>()
  {
    return m_saveArmed.get() ? "Armed" : "";
  }

}
