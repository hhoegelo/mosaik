#include "Controller.h"
#include "core/StepWizard.h"
#include "core/ParameterDescriptor.h"
#include <ui/Types.h>
#include <core/api/Interface.h>
#include <ui/midi-ui/Interface.h>
#include <dsp/api/display/Interface.h>
#include <ui/touch-ui/Interface.h>
#include <ui/ToolboxDefinition.h>
#include <cmath>

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
    auto tile = m_core.getSelectedTile();
    auto pattern = std::get<Core::Pattern>(m_core.getParameter(tile, Core::ParameterId::Pattern));

    pattern = Core::processWizard(
        pattern,
        static_cast<Core::WizardMode>(std::get<uint8_t>(m_core.getParameter(tile, Core::ParameterId::WizardMode))),
        static_cast<int8_t>(std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardRotate)))),
        static_cast<uint8_t>(std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardOns)))),
        static_cast<uint8_t>(std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardOffs)))));

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
                              auto fpp = m_touchUi.getToolboxes().getWaveform().getFramesPerPixel();
                              m_core.incParameter(tile, D::id, fpp * inc);
                            });
    //else
      //static_assert(false);
  }

  template <Toolbox T, typename D>
  std::pair<SoftButton, std::function<void()>> Controller::bindButtonUiParameterAction()
  {
    constexpr bool isGlobal = Core::GlobalParameters<Core::NoWrap>::contains(D::id);
    if constexpr(D::action == UiAction::Toggle)
      return std::make_pair(std::get<SoftButton>(D::position),
                            [this]() { m_core.toggleSelectedTilesParameter(D::id); });
    //else
      //static_assert(false);
  }

  template <Toolbox T, typename D> std::pair<SoftButton, std::function<void()>> Controller::bindButtonUiInvokeAction()
  {
    if constexpr(D::action == UiAction::Invoke)
      return std::make_pair(std::get<SoftButton>(D::position),
                            [this]() { this->invokeButtonAction<T, typename D::ID>(); });
    //else
      //static_assert(false);
  }

  template <Toolbox T, typename D> std::pair<Knob, std::function<void(int)>> Controller::bindKnobUiInvokeAction()
  {
    if constexpr(D::action == UiAction::Invoke)
      return std::make_pair(std::get<Knob>(D::position),
                            [this](int i) { this->invokeKnobAction<T, typename D::ID>(i); });
    //else
      //static_assert(false);
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
          //else
            //static_assert(false);
        });

    return mapping;
  }

  template <typename I> struct Bind
  {
    using ID = I;
    std::function<void()> handler;
  };

  template <> void Controller::invokeButtonAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::Cancel>()
  {
    auto sel = m_core.getSelectedTiles().front();
    m_core.setParameter(sel, Core::ParameterId::WizardMode, static_cast<uint8_t>(Core::WizardMode::Or));
    m_core.setParameter(sel, Core::ParameterId::WizardRotate, 0.0f);
    m_core.setParameter(sel, Core::ParameterId::WizardOns, 0.0f);
    m_core.setParameter(sel, Core::ParameterId::WizardOffs, 0.0f);
  }

  template <> void Controller::invokeButtonAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::Apply>()
  {
    auto tile = m_core.getSelectedTiles().front();
    auto p = std::get<Core::Pattern>(m_core.getParameter(tile, Core::ParameterId::Pattern));

    p = Core::processWizard(
        p, static_cast<Core::WizardMode>(std::get<uint8_t>(m_core.getParameter(tile, Core::ParameterId::WizardMode))),
        static_cast<int8_t>(std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardRotate)))),
        static_cast<uint8_t>(std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardOns)))),
        static_cast<uint8_t>(std::round(std::get<float>(m_core.getParameter(tile, Core::ParameterId::WizardOffs)))));

    m_core.setParameter(tile, Core::ParameterId::Pattern, p);
    this->invokeButtonAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::Cancel>();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::And>()
  {
    auto tile = m_core.getSelectedTile();
    m_core.setParameter(tile, Core::ParameterId::WizardMode, static_cast<uint8_t>(Core::WizardMode::And));
  }

  template <> void Controller::invokeButtonAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::Or>()
  {
    auto tile = m_core.getSelectedTile();
    m_core.setParameter(tile, Core::ParameterId::WizardMode, static_cast<uint8_t>(Core::WizardMode::Or));
  }

  template <> void Controller::invokeButtonAction<Toolbox::Steps, ToolboxDefinition<Toolbox::Steps>::Not>()
  {
    auto tile = m_core.getSelectedTile();
    m_core.setParameter(tile, Core::ParameterId::WizardMode, static_cast<uint8_t>(Core::WizardMode::Not));
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Up>()
  {
    m_touchUi.getToolboxes().getFileBrowser().dec();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Down>()
  {
    m_touchUi.getToolboxes().getFileBrowser().inc();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Leave>()
  {
    m_touchUi.getToolboxes().getFileBrowser().up();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Enter>()
  {
    m_touchUi.getToolboxes().getFileBrowser().down();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Prelisten>()
  {
    m_touchUi.getToolboxes().getFileBrowser().prelisten();
  }

  template <> void Controller::invokeButtonAction<Toolbox::Tile, ToolboxDefinition<Toolbox::Tile>::Load>()
  {
    m_touchUi.getToolboxes().getFileBrowser().load();
  }

  template <> void Controller::invokeKnobAction<Toolbox::Waveform, ToolboxDefinition<Toolbox::Waveform>::Zoom>(int inc)
  {
    m_touchUi.getToolboxes().getWaveform().incZoom(inc);
  }

  template <>
  void Controller::invokeKnobAction<Toolbox::Waveform, ToolboxDefinition<Toolbox::Waveform>::Scroll>(int inc)
  {
    m_touchUi.getToolboxes().getWaveform().incScroll(inc);
  }

  template <>
  void Controller::invokeKnobAction<Toolbox::Waveform, ToolboxDefinition<Toolbox::Waveform>::HitPoint>(int inc)
  {
    auto sel = m_core.getSelectedTiles().front();
    auto reverse = std::get<bool>(m_core.getParameter(sel, Core::ParameterId::Reverse));
    auto fpp = m_touchUi.getToolboxes().getWaveform().getFramesPerPixel();
    m_core.incParameter(sel, Core::ParameterId::TriggerFrame, reverse ? -fpp * inc : fpp * inc);
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
      m_midiUi.setLed(led, color);
    }
  }
}
