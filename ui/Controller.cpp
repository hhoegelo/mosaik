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
          {
            auto toolbox = p->getToolboxes().getSelectedToolbox();
            m_inputMapping = createMapping(toolbox);

            if(toolbox == Ui::Toolbox::ColorAdjust)
            {
              m_colorAdjustmentComputations = std::make_unique<Tools::DeferredComputations>();
              m_colorAdjustmentComputations->add(
                  [this]
                  {
                    for(auto button : { SoftButton::Right_SouthWest, SoftButton::Right_South,
                                        SoftButton::Right_SouthEast, SoftButton::Right_West, SoftButton::Right_Center,
                                        SoftButton::Right_East, SoftButton::Right_North, SoftButton::Right_NorthEast })
                    {
                      for(auto m : m_midiUi)
                        m->setLed(button, m_led_R, m_led_G, m_led_B);
                    }

                    if(auto t = m_touchUi.get())
                      t->setColorAdjustmentColor(m_screen_R, m_screen_G, m_screen_B);
                  });
            }
            else
            {
              m_colorAdjustmentComputations.reset();
            }
          }
        });

    m_computations.add([this] { showPattern(); });
  }

  void Controller::addUi(Midi::Interface &midiUI)
  {
    m_stepsLedLatch.fill(Color::Off);
    m_knobsLedLatch.fill(Color::Off);
    m_softButtonLedLatch.fill(Color::Off);

    m_midiUi.push_back(&midiUI);

    if(auto p = m_touchUi.get())
      m_inputMapping = createMapping(p->getToolboxes().getSelectedToolbox());
  }

  void Controller::showPattern()
  {
    auto tile = m_core.getSelectedTile();
    auto pattern = std::get<Core::Pattern>(m_core.getParameter(tile, Core::ParameterId::Pattern));

    for(size_t i = 0; i < 64; i++)
    {
      auto isCurrentStep = m_currentStep == i;
      auto isProgrammed = pattern[i];
      setLed(i, isCurrentStep ? Color::White : isProgrammed ? Color::Green : Color::Off);
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

  template <Ui::Toolbox T> Controller::Mapping Controller::buildMapping(Ui::Toolbox t)
  {
    if(t == T)
      return buildMapping<T>();
    else
      return buildMapping<static_cast<Ui::Toolbox>(static_cast<int>(T) + 1)>(t);
  }

  template <> Controller::Mapping Controller::buildMapping<Ui::Toolbox::NUM_TOOLBOXES>(Ui::Toolbox t)
  {
    throw std::invalid_argument("No such toolbox");
  }

  Controller::Mapping Controller::createMapping(Ui::Toolbox t)
  {
    return buildMapping<static_cast<Ui::Toolbox>(0)>(t);
  }

  template <typename D> std::pair<Knob, std::function<void(int)>> Controller::bindKnobUiParameterAction(float factor)
  {
    constexpr bool isGlobal = Core::GlobalParameters<Core::NoWrap>::contains(D::id);
    if constexpr(D::action == UiAction::IncDec)
      return std::make_pair(std::get<Knob>(D::position),
                            [this, factor](int inc)
                            {
                              auto tile = isGlobal ? Core::Address {} : m_core.getSelectedTile();
                              m_core.incParameter(tile, D::id, factor * inc);
                            });
    else if constexpr(D::action == UiAction::IncDecZoomed)
      return std::make_pair(std::get<Knob>(D::position),
                            [this, factor](int inc)
                            {
                              if(auto p = m_touchUi.get())
                              {
                                auto tile = isGlobal ? Core::Address {} : m_core.getSelectedTile();
                                auto fpp = p->getToolboxes().getWaveform().getFramesPerPixel();
                                m_core.incParameter(tile, D::id, factor * fpp * inc);
                              }
                            });
    else
      UNSUPPORTED_BRANCH();
  }

  template <typename D> std::pair<SoftButton, std::function<void()>> Controller::bindButtonUiParameterAction()
  {
    if constexpr(D::action == UiAction::Toggle)
      return std::make_pair(std::get<SoftButton>(D::position),
                            [this]() { m_core.toggleSelectedTilesParameter(D::id); });
    else if constexpr(D::action == UiAction::Invoke)
      return bindButtonUiInvokeAction<D>();
    else
      UNSUPPORTED_BRANCH();
  }

  template <typename D> std::pair<Knob, std::function<void()>> Controller::bindKnobUiClickAction()
  {
    if constexpr(D::action == UiAction::Default)
      return bindKnobUiDefaultClickAction<D>();
    else if constexpr(D::action == UiAction::Invoke)
      return std::make_pair(std::get<Knob>(D::position), [this]() { this->invokeKnobClickAction<typename D::ID>(); });
    else
      UNSUPPORTED_BRANCH();
  }

  template <typename D> std::pair<Knob, std::function<void()>> Controller::bindKnobUiDefaultClickAction()
  {
    constexpr bool isGlobal = Core::GlobalParameters<Core::NoWrap>::contains(D::id);
    auto tile = isGlobal ? Core::Address {} : m_core.getSelectedTile();
    return std::make_pair(std::get<Knob>(D::position), [this, tile]()
                          { m_core.setParameter(tile, D::id, ParameterDescriptor<D::id>::defaultValue); });
  }

  template <typename D> std::pair<SoftButton, std::function<void()>> Controller::bindButtonUiInvokeAction()
  {
    if constexpr(D::action == UiAction::Invoke)
      return std::make_pair(std::get<SoftButton>(D::position),
                            [this]() { this->invokeButtonAction<typename D::ID>(); });
    else
      UNSUPPORTED_BRANCH();
  }

  template <typename D> std::pair<Knob, std::function<void(int)>> Controller::bindKnobUiInvokeAction(float factor)
  {
    if constexpr(D::action == UiAction::Invoke)
      return std::make_pair(std::get<Knob>(D::position),
                            [this, factor](int i) { this->invokeKnobAction<typename D::ID>(factor * i); });
    else
      UNSUPPORTED_BRANCH();
  }

  template <Toolbox T> Controller::Mapping Controller::buildMapping()
  {
    Controller::Mapping mapping;

    for(auto a = static_cast<int>(Knob::FirstKnob); a <= static_cast<int>(Knob::LastKnob); a++)
      setLed(static_cast<Knob>(a), Color::Off);

    for(auto a = static_cast<int>(SoftButton::FirstButton); a <= static_cast<int>(SoftButton::LastButton); a++)
      setLed(static_cast<SoftButton>(a), Color::Off);

    ToolboxDefinition<T>::MaximizedParameters::forEach(
        [&](auto a)
        {
          using D = decltype(a);
          if constexpr(D::event == UiEvent::ReleasedKnobRotate)
          {
            mapping.knobIncDecReleased.insert(bindKnobUiParameterAction<D>());
            setLed(std::get<Knob>(D::position), D::color);

            using P = ParameterDescriptor<D::id>;

            if constexpr(requires(P) { P::defaultValue; })
            {
              mapping.knobClick.insert(bindKnobUiDefaultClickAction<D>());
            }

            if constexpr(requires(P) { P::acceleration; })
            {
              mapping.knobIncDecPressed.insert(bindKnobUiParameterAction<D>(P::acceleration));
            }
          }
          else if constexpr(D::event == UiEvent::PressedKnobRotate)
          {
            mapping.knobIncDecPressed.insert(bindKnobUiParameterAction<D>());
            setLed(std::get<Knob>(D::position), D::color);
          }
          else if constexpr(D::event == UiEvent::ButtonPress)
          {
            mapping.buttonPressed.insert(bindButtonUiParameterAction<D>());
            setLed(std::get<SoftButton>(D::position), D::color);
          }
          else if constexpr(D::event == UiEvent::ButtonRelease)
          {
            mapping.buttonReleased.insert(bindButtonUiParameterAction<D>());
            setLed(std::get<SoftButton>(D::position), D::color);
          }
          else if constexpr(D::event == UiEvent::KnobClick)
          {
            mapping.knobClick.insert(bindKnobUiClickAction<D>());
            setLed(std::get<Knob>(D::position), D::color);
          }
          else
            throw std::runtime_error("Unsupported ui binding");
        });

    ToolboxDefinition<T>::MaximizedCustom::forEach(
        [&](auto a)
        {
          using D = decltype(a);

          if constexpr(D::event == UiEvent::ButtonPress)
          {
            mapping.buttonPressed.insert(bindButtonUiInvokeAction<D>());
            setLed(std::get<SoftButton>(D::position), D::color);
          }
          else if constexpr(D::event == UiEvent::ButtonRelease)
          {
            mapping.buttonReleased.insert(bindButtonUiInvokeAction<D>());
            setLed(std::get<SoftButton>(D::position), D::color);
          }
          else if constexpr(D::event == UiEvent::ReleasedKnobRotate)
          {
            mapping.knobIncDecReleased.insert(bindKnobUiInvokeAction<D>());

            if constexpr(requires(D::ID) { D::ID::acceleration; })
            {
              mapping.knobIncDecPressed.insert(bindKnobUiInvokeAction<D>(D::ID::acceleration));
            }

            setLed(std::get<Knob>(D::position), D::color);
          }
          else if constexpr(D::event == UiEvent::PressedKnobRotate)
          {
            mapping.knobIncDecPressed.insert(bindKnobUiInvokeAction<D>());
            setLed(std::get<Knob>(D::position), D::color);
          }
          else if constexpr(D::event == UiEvent::KnobClick)
          {
            mapping.knobClick.insert(bindKnobUiClickAction<D>());
            setLed(std::get<Knob>(D::position), D::color);
          }
          else
            UNSUPPORTED_BRANCH();
        });

    return mapping;
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Global>::TapNSync>()
  {
    m_core.addTap();
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Tile>::Up>()
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getFileBrowser().dec();
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Tile>::Down>()
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getFileBrowser().inc();
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Tile>::Leave>()
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getFileBrowser().up();
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Tile>::Enter>()
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getFileBrowser().down();
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Tile>::Load>()
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getFileBrowser().load();
  }

  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::Tile>::IncDec>(int inc)
  {
    if(auto p = m_touchUi.get())
    {
      while(inc > 0)
      {
        p->getToolboxes().getFileBrowser().inc();
        inc--;
      }

      while(inc < 0)
      {
        p->getToolboxes().getFileBrowser().dec();
        inc++;
      }
    }
  }

  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::Waveform>::Zoom>(int inc)
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getWaveform().incZoom(inc);
  }

  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::Waveform>::Scroll>(int inc)
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getWaveform().incScroll(inc);
  }

  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::Waveform>::HitPoint>(int inc)
  {
    if(auto p = m_touchUi.get())
    {
      auto sel = m_core.getSelectedTile();
      auto reverse = std::get<bool>(m_core.getParameter(sel, Core::ParameterId::Reverse));
      auto fpp = p->getToolboxes().getWaveform().getFramesPerPixel();
      m_core.incParameter(sel, Core::ParameterId::TriggerFrame, reverse ? -fpp * inc : fpp * inc);
    }
  }

  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::Steps>::OneFitsAll>(int inc)
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

    if(m_wizardRotation > 0)
      std::rotate(pattern.rbegin(), pattern.rbegin() + std::abs(m_wizardRotation) % NUM_STEPS, pattern.rend());
    else if(m_wizardRotation < 0)
      std::rotate(pattern.begin(), pattern.begin() + std::abs(m_wizardRotation) % NUM_STEPS, pattern.end());

    m_core.setParameter(sel, Core::ParameterId::Pattern, pattern);
  }

  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::Steps>::Rotate>(int inc)
  {
    auto sel = m_core.getSelectedTile();
    auto pattern = std::get<Core::Pattern>(m_core.getParameter(sel, Core::ParameterId::Pattern));

    inc = std::clamp(inc, -(NUM_STEPS - 1), (NUM_STEPS - 1));

    if(inc > 0)
      std::rotate(pattern.rbegin(), pattern.rbegin() + std::abs(inc) % NUM_STEPS, pattern.rend());
    else if(inc < 0)
      std::rotate(pattern.begin(), pattern.begin() + std::abs(inc) % NUM_STEPS, pattern.end());

    m_wizardRotation = m_wizardRotation + inc;

    m_core.setParameter(sel, Core::ParameterId::Pattern, pattern);
  }

  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::Steps>::Steps>(int inc)
  {
    m_wizardSteps = std::clamp(m_wizardSteps + inc, 0, 64);
    processStepsGapsWizard();
  }

  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::Steps>::Gaps>(int inc)
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
        std::rotate(pattern.rbegin(), pattern.rbegin() + std::abs(m_wizardRotation) % NUM_STEPS, pattern.rend());
      else if(m_wizardRotation < 0)
        std::rotate(pattern.begin(), pattern.begin() + std::abs(m_wizardRotation) % NUM_STEPS, pattern.end());

      if(m_wizardInvert.get())
      {
        for(auto &a : pattern)
          a = !a;
      }

      m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern, pattern);
    }
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Steps>::All>()
  {
    Core::Pattern pattern = {};
    pattern.fill(true);
    m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern, pattern);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Steps>::None>()
  {
    Core::Pattern pattern = {};
    pattern.fill(false);
    m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern, pattern);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Steps>::Invert>()
  {
    m_wizardInvert = !m_wizardInvert.get();
    auto pattern = std::get<Core::Pattern>(m_core.getParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern));
    for(auto &a : pattern)
      a = !a;
    m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern, pattern);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Steps>::Mirror>()
  {
    m_wizardMirror = !m_wizardMirror.get();
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Mute>::LastMute>()
  {
    for(uint8_t t = 0; t < NUM_TILES; t++)
    {
      m_core.setParameter(Core::Address { t }, Core::ParameterId::Mute, m_savedMute[t]);
    }
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Mute>::SaveArmed>()
  {
    m_saveArmed = true;
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Mute>::SaveUnarmed>()
  {
    m_saveArmed = false;
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Mute>::Slot1>()
  {
    handleMuteSlot(0);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Mute>::Slot2>()
  {
    handleMuteSlot(1);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Mute>::Slot3>()
  {
    handleMuteSlot(2);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Mute>::Slot4>()
  {
    handleMuteSlot(3);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Mute>::Slot5>()
  {
    handleMuteSlot(4);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Mute>::Slot6>()
  {
    handleMuteSlot(5);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Mute>::UnmuteAll>()
  {
    for(uint8_t t = 0; t < NUM_TILES; t++)
    {
      m_savedMute[t] = std::get<bool>(m_core.getParameter(Core::Address { t }, Core::ParameterId::Mute));
      m_core.setParameter(Core::Address { t }, Core::ParameterId::Mute, false);
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

  void Controller::setLed(Knob knob, Color color)
  {
    auto idx = static_cast<size_t>(knob);

    if(std::exchange(m_knobsLedLatch[idx], color) != color)
    {
      for(auto a : m_midiUi)
        a->setLed(knob, color);
    }
  }

  void Controller::setLed(SoftButton s, Color color)
  {
    auto idx = static_cast<size_t>(s);

    if(std::exchange(m_softButtonLedLatch[idx], color) != color)
    {
      for(auto a : m_midiUi)
        a->setLed(s, color);
    }
  }

  void Controller::setLed(Step s, Color color)
  {
    auto idx = static_cast<size_t>(s);

    if(std::exchange(m_stepsLedLatch[idx], color) != color)
    {
      for(auto a : m_midiUi)
        a->setLed(s, color);
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
  bool fillString(std::string &target, Core::Api::Interface &core, Core::Address address, Core::ParameterId id)
  {
    if(Description::id == id)
    {
      if constexpr(requires(Description) { Description::format(typename Description::Type {}); })
      {
        target = Description::format(std::get<typename Description::Type>(core.getParameter(address, id)));
      }
      else
      {
        target = "";
      }

      return true;
    }

    return false;
  }

  std::string Controller::getDisplayValue(Core::Address address, Core::ParameterId id) const
  {
    std::string ret;
    if(!std::apply([&](auto... a) { return (fillString<decltype(a)>(ret, m_core, address, id) || ...); },
                   GlobalParameters {}))
      std::apply([&](auto... a) { return (fillString<decltype(a)>(ret, m_core, address, id) || ...); },
                 TileParameters {});
    return ret;
  }

  std::string Controller::getDisplayValue(Core::ParameterId id) const
  {
    return getDisplayValue(m_core.getSelectedTile(), id);
  }

  void Controller::handleMuteSlot(int slot)
  {
    if(m_saveArmed.get())
    {
      MuteState s;
      for(uint8_t t = 0; t < NUM_TILES; t++)
        s[t] = std::get<bool>(m_core.getParameter({ t }, Core::ParameterId::Mute));
      m_savedMutes[slot] = s;
    }
    else
    {
      MuteState s = m_savedMutes[slot];
      for(uint8_t t = 0; t < NUM_TILES; t++)
        m_core.setParameter({ t }, Core::ParameterId::Mute, s[t]);
    }
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::Waveform>::Zoom) const
  {
    if(auto p = m_touchUi.get())
      return Tools::format("x %3.2f", p->getToolboxes().getWaveform().getZoom());
    return "";
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::Waveform>::Scroll) const
  {
    if(auto p = m_touchUi.get())
      return Tools::format("%" PRId64 " frames", p->getToolboxes().getWaveform().getScroll());
    return "";
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::Waveform>::HitPoint) const
  {
    return Tools::format(
        "%" PRId64 " frames",
        std::get<Core::FramePos>(m_core.getParameter(m_core.getSelectedTile(), Core::ParameterId::TriggerFrame)));
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::Steps>::Gaps) const
  {
    return std::to_string(m_wizardGaps);
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::Steps>::Steps) const
  {
    return std::to_string(m_wizardSteps);
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::Steps>::Rotate) const
  {
    return std::to_string(m_wizardRotation);
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::Steps>::Invert) const
  {
    return m_wizardInvert.get() ? "On" : "Off";
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::Steps>::Mirror) const
  {
    return m_wizardMirror.get() ? "On" : "Off";
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::Mute>::SaveArmed) const
  {
    return m_saveArmed.get() ? "Armed" : "";
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::Mute>::SaveUnarmed) const
  {
    return m_saveArmed.get() ? "Armed" : "";
  }

  // Color Adjustment
  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::ColorAdjust>::Led_R>(int inc)
  {
    m_led_R = std::clamp(m_led_R.get() + inc, 0, 127);
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::ColorAdjust>::Led_R) const
  {
    return std::to_string(m_led_R.get() * 2);
  }

  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::ColorAdjust>::Led_G>(int inc)
  {
    m_led_G = std::clamp(m_led_G.get() + inc, 0, 127);
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::ColorAdjust>::Led_G) const
  {
    return std::to_string(m_led_G.get() * 2);
  }

  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::ColorAdjust>::Led_B>(int inc)
  {
    m_led_B = std::clamp(m_led_B.get() + inc, 0, 127);
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::ColorAdjust>::Led_B) const
  {
    return std::to_string(m_led_B.get() * 2);
  }

  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::ColorAdjust>::Screen_R>(int inc)
  {
    m_screen_R = std::clamp(m_screen_R.get() + inc, 0, 255);
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::ColorAdjust>::Screen_R) const
  {
    return std::to_string(m_screen_R.get());
  }

  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::ColorAdjust>::Screen_G>(int inc)
  {
    m_screen_G = std::clamp(m_screen_G.get() + inc, 0, 255);
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::ColorAdjust>::Screen_G) const
  {
    return std::to_string(m_screen_G.get());
  }

  template <> void Controller::invokeKnobAction<ToolboxDefinition<Toolbox::ColorAdjust>::Screen_B>(int inc)
  {
    m_screen_B = std::clamp(m_screen_B.get() + inc, 0, 255);
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::ColorAdjust>::Screen_B) const
  {
    return std::to_string(m_screen_B.get());
  }

  template <> void Controller::invokeButtonAction<PreviousToolbox>()
  {
    if(auto t = m_touchUi.get())
    {
      auto s = static_cast<int>(t->getToolboxes().getSelectedToolbox());
      if(s == 0)
        s = static_cast<int>(Ui::Toolbox::NUM_TOOLBOXES) - 1;
      else
        s = s - 1;

      t->getToolboxes().selectToolbox(static_cast<Ui::Toolbox>(s));
    }
  }

  template <> void Controller::invokeButtonAction<NextToolbox>()
  {
    if(auto t = m_touchUi.get())
    {
      auto s = static_cast<int>(t->getToolboxes().getSelectedToolbox());
      if(s == static_cast<int>(Ui::Toolbox::NUM_TOOLBOXES) - 1)
        s = 0;
      else
        s = s + 1;

      t->getToolboxes().selectToolbox(static_cast<Ui::Toolbox>(s));
    }
  }

  template <> void Controller::invokeKnobClickAction<ToolboxDefinition<Toolbox::Tile>::Load>()
  {
    if(auto p = m_touchUi.get())
      p->getToolboxes().getFileBrowser().load();
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Global>::StartSlowDown>()
  {
    m_core.setParameter({}, Core::ParameterId::GlobalTempoMultiplier,
                        Core::ParameterDescriptor<Core::ParameterId::GlobalTempoMultiplier>::min);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Global>::StopSlowDown>()
  {
    m_core.setParameter({}, Core::ParameterId::GlobalTempoMultiplier,
                        Core::ParameterDescriptor<Core::ParameterId::GlobalTempoMultiplier>::defaultValue);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Global>::StartSpeedUp>()
  {
    m_core.setParameter({}, Core::ParameterId::GlobalTempoMultiplier,
                        Core::ParameterDescriptor<Core::ParameterId::GlobalTempoMultiplier>::max);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Global>::StopSpeedUp>()
  {
    m_core.setParameter({}, Core::ParameterId::GlobalTempoMultiplier,
                        Core::ParameterDescriptor<Core::ParameterId::GlobalTempoMultiplier>::defaultValue);
  }
}
