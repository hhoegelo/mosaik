#include "Controller.h"
#include "core/ParameterDescriptor.h"
#include "Types.h"
#include "core/api/Interface.h"
#include "ui/midi-ui/Interface.h"
#include "dsp/api/display/Interface.h"
#include "ui/touch-ui/Interface.h"
#include "ToolboxDefinition.h"
#include <ui/StepWizard.h>
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
            /*
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
            */
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

  static Color stepTypeToColor(Core::StepType s)
  {
    switch(s)
    {
      case Core::StepType::Empty:
        return Color::Off;
      case Core::StepType::Trigger:
        return Color::Green;
      case Core::StepType::Triplet:
        return Color::Yellow;
      case Core::StepType::Skip:
        return Color::Red;
    }
    return Color::Off;
  }

  void Controller::showPattern()
  {
    auto tile = m_core.getSelectedTile();
    auto pattern = std::get<Core::Pattern>(m_core.getParameter(tile, Core::ParameterId::Pattern));

    for(uint8_t i = 0; i < NUM_STEPS; i++)
    {
      auto isCurrentStep = m_currentStep == i;
      auto isTriplet = m_tripletArmed.get() && isTripletStep(i);
      auto c = (isTriplet && pattern[i] != Core::StepType::Triplet) ? Color::LightBlue : stepTypeToColor(pattern[i]);
      setLed(i, isCurrentStep ? Color::White : c);
    }
  }

  bool Controller::isTripletStep(uint8_t i) const
  {
    return ((i + 1) % 4 == 0);
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

  template <> void Controller::invokeButtonAction<GotoToolboxGlobal>()
  {
    if(auto t = m_touchUi.get())
    {
      t->getToolboxes().selectToolbox(Ui::Toolbox::Global);
    }
  }

  template <> void Controller::invokeButtonAction<GotoToolboxWaveform>()
  {
    if(auto t = m_touchUi.get())
    {
      t->getToolboxes().selectToolbox(Ui::Toolbox::Waveform);
    }
  }

  template <> void Controller::invokeButtonAction<GotoToolboxTile>()
  {
    if(auto t = m_touchUi.get())
    {
      t->getToolboxes().selectToolbox(Ui::Toolbox::Tile);
    }
  }

  template <> void Controller::invokeButtonAction<GotoToolboxMute>()
  {
    if(auto t = m_touchUi.get())
    {
      t->getToolboxes().selectToolbox(Ui::Toolbox::Mute);
    }
  }

  template <> void Controller::invokeButtonAction<GotoToolboxSteps>()
  {
    if(auto t = m_touchUi.get())
    {
      t->getToolboxes().selectToolbox(Ui::Toolbox::Steps);
    }
  }

  template <> void Controller::invokeButtonAction<GotoToolboxSnapshots>()
  {
    if(auto t = m_touchUi.get())
    {
      t->getToolboxes().selectToolbox(Ui::Toolbox::Snapshots);
    }
  }

  template <> void Controller::invokeButtonAction<GotoToolboxReverb>()
  {
    if(auto t = m_touchUi.get())
    {
      t->getToolboxes().selectToolbox(Ui::Toolbox::Reverb);
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

  template <typename T> constexpr bool isParameter()
  {
    if constexpr(requires { std::is_same_v<ParameterDescriptor<T::ID::id>, typename T::ID>; })
      return std::is_same_v<ParameterDescriptor<T::ID::id>, typename T::ID>;
    else
      return false;
  }

  template <Toolbox T> Controller::Mapping Controller::buildMapping()
  {
    Controller::Mapping mapping;

    for(auto a = static_cast<int>(Knob::FirstKnob); a <= static_cast<int>(Knob::LastKnob); a++)
      setLed(static_cast<Knob>(a), Color::Off);

    for(auto a = static_cast<int>(SoftButton::FirstButton); a <= static_cast<int>(SoftButton::LastButton); a++)
      setLed(static_cast<SoftButton>(a), Color::Off);

    ToolboxDefinition<T>::Entires::forEach(
        [&](auto a)
        {
          using D = decltype(a);

          if constexpr(isParameter<D>())
          {
            if constexpr(D::action == UiAction::Invoke)
            {
              if constexpr(D::event == UiEvent::ReleasedKnobRotate)
              {
                mapping.knobIncDecReleased[std::get<Knob>(D::position)]
                    = [this](int inc) { invokeKnobAction<D::ID>(inc); };

                if constexpr(requires { D::ID::acceleration; })
                {
                  mapping.knobIncDecPressed[std::get<Knob>(D::position)]
                      = [this](int inc) { invokeKnobAction<D::ID>(D::ID::acceleration * inc); };
                }
                setLed(std::get<Knob>(D::position), D::color);
              }
              else if constexpr(D::event == UiEvent::ButtonPress)
              {
                mapping.buttonPressed[std::get<SoftButton>(D::position)]
                    = [this](int inc) { this->invokeButtonAction<D::ID>(); };
                setLed(std::get<SoftButton>(D::position), D::color);
              }
              else if constexpr(D::event == UiEvent::ButtonRelease)
              {
                mapping.buttonReleased[std::get<SoftButton>(D::position)]
                    = [this](int inc) { this->invokeButtonAction<D::ID>(); };
                setLed(std::get<SoftButton>(D::position), D::color);
              }
              else
              {
                UNSUPPORTED_BRANCH();
              }
            }
            else if constexpr(D::event == UiEvent::ButtonPress && D::action == UiAction::Toggle)
            {
              mapping.buttonPressed[std::get<SoftButton>(D::position)]
                  = [this] { m_core.toggleSelectedTilesParameter(D::ID::id); };
              setLed(std::get<SoftButton>(D::position), D::color);
            }
            else if constexpr(D::event == UiEvent::ReleasedKnobRotate && D::action == UiAction::IncDec)
            {
              mapping.knobIncDecReleased[std::get<Knob>(D::position)]
                  = [this](int inc) { m_core.incParameter(m_core.getSelectedTile(), D::ID::id, inc); };

              if constexpr(requires { D::ID::defaultValue; })
              {
                mapping.knobClick[std::get<Knob>(D::position)]
                    = [this] { m_core.setParameter(m_core.getSelectedTile(), D::ID::id, D::ID::defaultValue); };
              }

              if constexpr(requires { D::ID::acceleration; })
              {
                mapping.knobIncDecPressed[std::get<Knob>(D::position)] = [this](int inc)
                { m_core.incParameter(m_core.getSelectedTile(), D::ID::id, D::ID::acceleration * inc); };
              }
              setLed(std::get<Knob>(D::position), D::color);
            }
            else if constexpr(D::event == UiEvent::ReleasedKnobRotate && D::action == UiAction::IncDecZoomed)
            {
              mapping.knobIncDecReleased[std::get<Knob>(D::position)] = [this](int inc)
              {
                m_core.incParameter(m_core.getSelectedTile(), D::ID::id,
                                    m_touchUi.get()->getToolboxes().getWaveform().getFramesPerPixel() * inc);
              };
              setLed(std::get<Knob>(D::position), D::color);
            }
            else
            {
              UNSUPPORTED_BRANCH();
            }
          }
          else
          {
            if constexpr(D::action == UiAction::Invoke)
            {
              if constexpr(D::event == UiEvent::KnobClick)
              {
                mapping.knobClick[std::get<Knob>(D::position)] = [this] { invokeButtonAction<typename D::ID>(); };
                setLed(std::get<Knob>(D::position), D::color);
              }
              else if constexpr(D::event == UiEvent::ReleasedKnobRotate)
              {
                mapping.knobIncDecReleased[std::get<Knob>(D::position)]
                    = [this](int inc) { invokeKnobAction<typename D::ID>(inc); };

                if constexpr(requires { D::ID::acceleration; })
                {
                  mapping.knobIncDecPressed[std::get<Knob>(D::position)]
                      = [this](int inc) { invokeKnobAction<typename D::ID>(D::ID::acceleration * inc); };
                }

                setLed(std::get<Knob>(D::position), D::color);
              }
              else if constexpr(D::event == UiEvent::ButtonPress)
              {
                mapping.buttonPressed[std::get<SoftButton>(D::position)]
                    = [this] { invokeButtonAction<typename D::ID>(); };
                setLed(std::get<SoftButton>(D::position), D::color);
              }
              else if constexpr(D::event == UiEvent::ButtonRelease)
              {
                mapping.buttonReleased[std::get<SoftButton>(D::position)]
                    = [this] { invokeButtonAction<typename D::ID>(); };
                setLed(std::get<SoftButton>(D::position), D::color);
              }
              else
              {
                UNSUPPORTED_BRANCH();
              }
            }
            else
            {
              UNSUPPORTED_BRANCH();
            }
          }
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
    static auto s_patterns = buildPatterns();
    m_oneFitsAllStepWizard = std::clamp<int>(m_oneFitsAllStepWizard + inc, 0, s_patterns.size() - 1);

    auto sel = m_core.getSelectedTile();
    auto pattern = s_patterns[m_oneFitsAllStepWizard];
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
            pattern[i] = Core::StepType::Trigger;

          i++;
        }

        i += m_wizardGaps;
      }

      if(m_wizardRotation > 0)
        std::rotate(pattern.rbegin(), pattern.rbegin() + std::abs(m_wizardRotation) % NUM_STEPS, pattern.rend());
      else if(m_wizardRotation < 0)
        std::rotate(pattern.begin(), pattern.begin() + std::abs(m_wizardRotation) % NUM_STEPS, pattern.end());

      if(m_wizardInvert.get())
        invert(pattern);

      m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern, pattern);
    }
  }

  void Controller::invert(Core::Pattern &pattern) const
  {
    for(auto i = 0; i < NUM_STEPS; i++)
      pattern[i] = invert(i, pattern[i]);
  }

  Core::StepType Controller::invert(uint8_t pos, Core::StepType &a) const
  {
    if(a == Core::StepType::Trigger)
      return Core::StepType::Empty;
    else if(a == Core::StepType::Triplet)
      return Core::StepType::Empty;
    else if(a == Core::StepType::Empty)
      return (isTripletStep(pos) && m_tripletArmed.get()) ? Core::StepType::Triplet : Core::StepType::Trigger;
    return a;
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Steps>::All>()
  {
    auto pattern = std::get<Core::Pattern>(m_core.getParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern));
    std::replace(pattern.begin(), pattern.end(), Core::StepType::Empty, Core::StepType::Trigger);
    m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern, pattern);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Steps>::None>()
  {
    auto pattern = std::get<Core::Pattern>(m_core.getParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern));
    std::replace(pattern.begin(), pattern.end(), Core::StepType::Trigger, Core::StepType::Empty);
    m_core.setParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern, pattern);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Steps>::Invert>()
  {
    m_wizardInvert = !m_wizardInvert.get();
    auto pattern = std::get<Core::Pattern>(m_core.getParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern));
    invert(pattern);
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
      auto newState = invert(b, state);
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
        a->setLed(knob, color, Brightness::Tenth);
    }
  }

  void Controller::setLed(SoftButton s, Color color)
  {
    auto idx = static_cast<size_t>(s);

    if(std::exchange(m_softButtonLedLatch[idx], color) != color)
    {
      for(auto a : m_midiUi)
        a->setLed(s, color, Brightness::Tenth);
    }
  }

  void Controller::setLed(Step s, Color color)
  {
    auto idx = static_cast<size_t>(s);

    if(std::exchange(m_stepsLedLatch[idx], color) != color)
    {
      for(auto a : m_midiUi)
        a->setLed(s, color, Brightness::Tenth);
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
        target = Description::format(std::get<typename Description::Type>(core.getParameter(address, id)));
      else
        target = "";

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
      return Tools::format("%" PRId64 " frms", p->getToolboxes().getWaveform().getScroll());
    return "";
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::Waveform>::HitPoint) const
  {
    return Tools::format(
        "%" PRId64 " frms",
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

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Steps>::ArmSkipStep>()
  {
    m_skipStepArmed = !m_skipStepArmed.get();
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Steps>::DisarmSkipStep>()
  {
    //m_skipStepArmed = false;
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Steps>::ArmTriplet>()
  {
    m_tripletArmed = !m_tripletArmed.get();
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Steps>::DisarmTriplet>()
  {
    //  m_tripletArmed = false;
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::Mute>::SaveArmed) const
  {
    return m_saveArmed.get() ? "Armed" : "";
  }

  std::string Controller::getDisplayValue(ToolboxDefinition<Toolbox::Mute>::SaveUnarmed) const
  {
    return m_saveArmed.get() ? "Armed" : "";
  }

  /*
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
*/
  template <> void Controller::invokeButtonAction<PreviousToolbox>()
  {
    if(auto t = m_touchUi.get())
    {
      auto s = static_cast<int>(t->getToolboxes().getSelectedToolbox());
      if(s != 0)
        s--;

      t->getToolboxes().selectToolbox(static_cast<Ui::Toolbox>(s));
    }
  }

  template <> void Controller::invokeButtonAction<NextToolbox>()
  {
    if(auto t = m_touchUi.get())
    {
      auto s = static_cast<int>(t->getToolboxes().getSelectedToolbox());
      if(s != static_cast<int>(Ui::Toolbox::NUM_TOOLBOXES) - 1)
        s++;

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

  // Snapshots
  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Snapshots>::SaveArmed>()
  {
    m_snapshotSaveArmed = true;
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Snapshots>::SaveUnarmed>()
  {
    m_snapshotSaveArmed = false;
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Snapshots>::Slot1>()
  {
    handleSnapshotSlot(0);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Snapshots>::Slot2>()
  {
    handleSnapshotSlot(1);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Snapshots>::Slot3>()
  {
    handleSnapshotSlot(2);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Snapshots>::Slot4>()
  {
    handleSnapshotSlot(3);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Snapshots>::Slot5>()
  {
    handleSnapshotSlot(4);
  }

  template <> void Controller::invokeButtonAction<ToolboxDefinition<Toolbox::Snapshots>::Slot6>()
  {
    handleSnapshotSlot(5);
  }

  void Controller::handleSnapshotSlot(int slot)
  {
    if(m_snapshotSaveArmed.get())
    {
      m_core.saveSnapshot(slot);
    }
    else
    {
      m_core.loadSnapshot(slot);
    }
  }
}
