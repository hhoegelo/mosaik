#pragma once

#include <ui/Types.h>
#include <ui/ParameterDescriptor.h>

namespace Ui
{

  template <Toolbox t> struct ToolboxDefinition;

  template <Core::ParameterId ID> struct MinimizedParameterEntry
  {
    constexpr static auto id = ID;
  };

  enum class UiEvent
  {
    ReleasedKnobRotate,
    PressedKnobRotate,
    ButtonPress,
    ButtonRelease
  };

  enum class UiAction
  {
    IncDec,
    IncDecZoomed,
    Set,
    Toggle,
    Invoke,
  };

  template <Core::ParameterId ID, Color C, UiEvent E, UiAction A, auto P> struct MaximizedParameterEntry
  {
    constexpr static auto id = ID;
    constexpr static Color color = C;
    constexpr static UiEvent event = E;
    constexpr static UiAction action = A;
    constexpr static std::variant<SoftButton, Knob> position = P;
  };

  template <typename I, Color C, UiEvent E, UiAction A, auto P> struct MaximizedCustomEntry
  {
    using ID = I;
    constexpr static Color color = C;
    constexpr static UiEvent event = E;
    constexpr static UiAction action = A;
    constexpr static std::variant<SoftButton, Knob> position = P;
  };

  template <typename... E> struct Entries
  {
    template <typename CB> static void forEach(const CB &cb)
    {
      (cb(E {}), ...);
    }
  };

  template <> struct ToolboxDefinition<Toolbox::Global>
  {
    constexpr static auto title = "Global";

    using Minimized = Entries<MinimizedParameterEntry<Core::ParameterId::GlobalVolume>,
                              MinimizedParameterEntry<Core::ParameterId::GlobalTempo>>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<Core::ParameterId::GlobalVolume, Color::Green, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<Core::ParameterId::GlobalTempo, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Leftmost>>;
    using MaximizedCustom = Entries<>;
  };

  template <> struct ToolboxDefinition<Toolbox::MainPlayground>
  {
    constexpr static auto title = "Main Playground";
    using Minimized = Entries<MinimizedParameterEntry<Core::ParameterId::MainPlayground1>,
                              MinimizedParameterEntry<Core::ParameterId::MainPlayground2>,
                              MinimizedParameterEntry<Core::ParameterId::MainPlayground3>>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<Core::ParameterId::MainPlayground1, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Leftmost>,
                  MaximizedParameterEntry<Core::ParameterId::MainPlayground2, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Rightmost>,
                  MaximizedParameterEntry<Core::ParameterId::MainPlayground3, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthWest>,
                  MaximizedParameterEntry<Core::ParameterId::MainPlayground4, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthEast>,
                  MaximizedParameterEntry<Core::ParameterId::MainPlayground5, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<Core::ParameterId::MainPlayground6, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::SouthWest>,
                  MaximizedParameterEntry<Core::ParameterId::MainPlayground7, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::SouthEast>>;
    using MaximizedCustom = Entries<>;
  };

  template <> struct ToolboxDefinition<Toolbox::Playground>
  {
    constexpr static auto title = "Playground";
    using Minimized = Entries<MinimizedParameterEntry<Core::ParameterId::Playground1>,
                              MinimizedParameterEntry<Core::ParameterId::Playground2>,
                              MinimizedParameterEntry<Core::ParameterId::Playground3>>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<Core::ParameterId::Playground1, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Leftmost>,
                  MaximizedParameterEntry<Core::ParameterId::Playground2, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Rightmost>,
                  MaximizedParameterEntry<Core::ParameterId::Playground3, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthWest>,
                  MaximizedParameterEntry<Core::ParameterId::Playground4, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthEast>,
                  MaximizedParameterEntry<Core::ParameterId::Playground5, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<Core::ParameterId::Playground6, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::SouthWest>,
                  MaximizedParameterEntry<Core::ParameterId::Playground7, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::SouthEast>>;
    using MaximizedCustom = Entries<>;
  };

  template <> struct ToolboxDefinition<Toolbox::Steps>
  {
    struct Cancel
    {
      static constexpr auto title = "Cancel";
    };

    struct Apply
    {
      static constexpr auto title = "Apply";
    };

    struct And
    {
      static constexpr auto title = "And";
    };

    struct Or
    {
      static constexpr auto title = "Or";
    };

    struct Not
    {
      static constexpr auto title = "Not";
    };

    constexpr static auto title = "Step Wizard";
    using Minimized = Entries<MinimizedParameterEntry<Core::ParameterId::WizardRotate>>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<Core::ParameterId::WizardRotate, Color::Green, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<Core::ParameterId::WizardOns, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::SouthEast>,
                  MaximizedParameterEntry<Core::ParameterId::WizardOffs, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::SouthWest>>;
    using MaximizedCustom = Entries<
        MaximizedCustomEntry<And, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
        MaximizedCustomEntry<Or, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_NorthEast>,
        MaximizedCustomEntry<Not, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_East>,
        MaximizedCustomEntry<Apply, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
        MaximizedCustomEntry<Cancel, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_South>>;
  };

  template <> struct ToolboxDefinition<Toolbox::Tile>
  {
    constexpr static auto title = "Tile";

    using Minimized
        = Entries<MinimizedParameterEntry<Core::ParameterId::Gain>, MinimizedParameterEntry<Core::ParameterId::Mute>,
                  MinimizedParameterEntry<Core::ParameterId::Balance>,
                  MinimizedParameterEntry<Core::ParameterId::Speed>>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<Core::ParameterId::Gain, Color::Green, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<Core::ParameterId::Speed, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Rightmost>,
                  MaximizedParameterEntry<Core::ParameterId::Balance, Color::Purple, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Leftmost>,
                  MaximizedParameterEntry<Core::ParameterId::Shuffle, Color::Red, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthEast>,
                  MaximizedParameterEntry<Core::ParameterId::Reverse, Color::Blue, UiEvent::ButtonPress,
                                          UiAction::Toggle, SoftButton::Left_Center>>;

    struct Up
    {
      static constexpr auto title = "Up";
    };

    struct Down
    {
      static constexpr auto title = "Down";
    };

    struct Enter
    {
      static constexpr auto title = "Enter";
    };

    struct Leave
    {
      static constexpr auto title = "Leave";
    };

    struct Load
    {
      static constexpr auto title = "Load";
    };

    struct Prelisten
    {
      static constexpr auto title = "Prelisten";
    };

    using MaximizedCustom = Entries<
        MaximizedCustomEntry<Up, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
        MaximizedCustomEntry<Down, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_South>,
        MaximizedCustomEntry<Leave, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_West>,
        MaximizedCustomEntry<Enter, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_East>,
        MaximizedCustomEntry<Load, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
        MaximizedCustomEntry<Prelisten, Color::Purple, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Right_SouthEast>>;
  };

  template <> struct ToolboxDefinition<Toolbox::Waveform>
  {
    constexpr static auto title = "Waveform";
    using Minimized = Entries<>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<Core::ParameterId::EnvelopeFadeInPos, Color::Blue,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::SouthWest>,
                  MaximizedParameterEntry<Core::ParameterId::EnvelopeFadedInPos, Color::Blue,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::NorthWest>,
                  MaximizedParameterEntry<Core::ParameterId::EnvelopeFadeOutPos, Color::Blue,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::NorthEast>,
                  MaximizedParameterEntry<Core::ParameterId::EnvelopeFadedOutPos, Color::Blue,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::SouthEast>>;

    struct Zoom
    {
      static constexpr auto title = "Zoom";
    };

    struct Scroll
    {
      static constexpr auto title = "Scroll";
    };

    struct HitPoint
    {
      static constexpr auto title = "Hit Point";
    };

    using MaximizedCustom = Entries<
        MaximizedCustomEntry<Zoom, Color::Purple, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Leftmost>,
        MaximizedCustomEntry<Scroll, Color::Purple, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Rightmost>,
        MaximizedCustomEntry<HitPoint, Color::Purple, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Center>>;
  };
}
