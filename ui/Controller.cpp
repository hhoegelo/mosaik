#include "Controller.h"
#include "core/ParameterDescriptor.h"
#include "Types.h"
#include "core/api/Interface.h"
#include "ui/midi-ui/Interface.h"
#include "dsp/api/display/Interface.h"
#include "ui/touch-ui/Interface.h"
#include "ToolboxDefinition.h"
#include <inttypes.h>
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

    m_computations.add([this] { m_inputMapping = createMapping(m_touchUi->getToolboxes().getSelectedToolbox()); });
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
      if(auto it = m_inputMapping.buttonPressed.find(b); it != m_inputMapping.buttonPressed.end())
        it->second();

    if(e == ButtonEvent::Release)
      if(auto it = m_inputMapping.buttonReleased.find(b); it != m_inputMapping.buttonReleased.end())
        it->second();
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
    }
    throw std::runtime_error("unknown toolbox");
  }

  template <Toolbox T, typename D> std::pair<Knob, std::function<void(int)>> Controller::bindKnobUiParameterAction()
  {
    constexpr bool isGlobal = Core::GlobalParameters<Core::NoWrap>::contains(D::id);
    if constexpr(D::action == UiAction::IncDec)
      return std::make_pair(std::get<Knob>(D::position),
                            [this](int inc)
                            {
                              auto tile = isGlobal ? Core::TileId {} : m_core.getSelectedTile();
                              m_core.incParameter(tile, D::id, inc);
                            });
    else if constexpr(D::action == UiAction::IncDecZoomed)
      return std::make_pair(std::get<Knob>(D::position),
                            [this](int inc)
                            {
                              auto tile = isGlobal ? Core::TileId {} : m_core.getSelectedTile();
                              auto fpp = m_touchUi->getToolboxes().getWaveform().getFramesPerPixel();
                              m_core.incParameter(tile, D::id, fpp * inc);
                            });
    else
      UNSUPPORTED_BRANCH();
  }

  template <Toolbox T, typename D>
  std::pair<SoftButton, std::function<void()>> Controller::bindButtonUiParameterAction()
  {
    constexpr bool isGlobal = Core::GlobalParameters<Core::NoWrap>::contains(D::id);
    if constexpr(D::action == UiAction::Toggle)
      return std::make_pair(std::get<SoftButton>(D::position),
                            [this]() { m_core.toggleSelectedTilesParameter(D::id); });
    else
      UNSUPPORTED_BRANCH();
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
            mapping.knobIncDecReleased.insert(bindKnobUiParameterAction<T, D>());
          else if constexpr(D::event == UiEvent::PressedKnobRotate)
            mapping.knobIncDecPressed.insert(bindKnobUiParameterAction<T, D>());
          else if constexpr(D::event == UiEvent::ButtonPress)
            mapping.buttonPressed.insert(bindButtonUiParameterAction<T, D>());
          else if constexpr(D::event == UiEvent::ButtonRelease)
            mapping.buttonReleased.insert(bindButtonUiParameterAction<T, D>());
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
    m_touchUi->getToolboxes().getFileBrowser().dec();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Down>()
  {
    m_touchUi->getToolboxes().getFileBrowser().inc();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Leave>()
  {
    m_touchUi->getToolboxes().getFileBrowser().up();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Enter>()
  {
    m_touchUi->getToolboxes().getFileBrowser().down();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Prelisten>()
  {
    m_touchUi->getToolboxes().getFileBrowser().prelisten();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Load>()
  {
    m_touchUi->getToolboxes().getFileBrowser().load();
  }

  template <> void Controller::invokeKnobAction<Toolbox::Waveform, ToolboxDefinition<Toolbox::Waveform>::Zoom>(int inc)
  {
    m_touchUi->getToolboxes().getWaveform().incZoom(inc);
  }

  template <>
  void Controller::invokeKnobAction<Toolbox::Waveform, ToolboxDefinition<Toolbox::Waveform>::Scroll>(int inc)
  {
    m_touchUi->getToolboxes().getWaveform().incScroll(inc);
  }

  template <>
  void Controller::invokeKnobAction<Toolbox::Waveform, ToolboxDefinition<Toolbox::Waveform>::HitPoint>(int inc)
  {
    auto sel = m_core.getSelectedTiles().front();
    auto reverse = std::get<bool>(m_core.getParameter(sel, Core::ParameterId::Reverse));
    auto fpp = m_touchUi->getToolboxes().getWaveform().getFramesPerPixel();
    m_core.incParameter(sel, Core::ParameterId::TriggerFrame, reverse ? -fpp * inc : fpp * inc);
  }

  template <> void Controller::invokeKnobAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::OneFitsAll>(int inc)
  {
    auto now = std::chrono::system_clock::now();

    m_stepWizardLastUsage = now;
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

  void Controller::onStepButtonEvent(Step b, ButtonEvent e)
  {
    if(e == ButtonEvent::Press)
    {
      auto merged = std::get<Core::Pattern>(m_core.getParameter(m_core.getSelectedTile(), Core::ParameterId::Pattern));
      auto state = merged[b];
      m_core.setStep(b, !state);
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

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Waveform>::Zoom>()
  {
    return Tools::format("x %3.2f", m_touchUi->getToolboxes().getWaveform().getZoom());
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Waveform>::Scroll>()
  {
    return Tools::format("%" PRId64 " frames", m_touchUi->getToolboxes().getWaveform().getScroll());
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Waveform>::HitPoint>()
  {
    return Tools::format(
        "%" PRId64 " frames",
        std::get<Core::FramePos>(m_core.getParameter(m_core.getSelectedTile(), Core::ParameterId::TriggerFrame)));
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Global>::TapNSync>()
  {
    return "";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Tile>::Up>()
  {
    return "";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Tile>::Down>()
  {
    return "";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Tile>::Enter>()
  {
    return "";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Tile>::Leave>()
  {
    return "";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Tile>::Load>()
  {
    return "";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Tile>::Prelisten>()
  {
    return "";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Steps>::OneFitsAll>()
  {
    return "";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Steps>::All>()
  {
    return "";
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
    return "";
  }

  template <> std::string Controller::getDisplayValue<ToolboxDefinition<Toolbox::Steps>::None>()
  {
    return "";
  }
}
