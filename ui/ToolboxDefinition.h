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
    ButtonRelease,
    KnobClick  // no turn between press and release
  };

  enum class UiAction
  {
    IncDec,
    IncDecZoomed,
    Set,
    Toggle,
    Invoke,
    Default
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

    struct TapNSync
    {
      constexpr static auto title = "Tap'n Sync";
    };

    using Minimized = Entries<MinimizedParameterEntry<Core::ParameterId::GlobalVolume>,
                              MinimizedParameterEntry<Core::ParameterId::GlobalTempo>>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<Core::ParameterId::GlobalVolume, Color::Green, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<Core::ParameterId::GlobalTempo, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Leftmost>>;
    using MaximizedCustom = Entries<
        MaximizedCustomEntry<TapNSync, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>>;
  };

  template <> struct ToolboxDefinition<Toolbox::MainPlayground>
  {
    constexpr static auto title = "Main Playground";
    using Minimized = Entries<MinimizedParameterEntry<Core::ParameterId::MainPlayground1>,
                              MinimizedParameterEntry<Core::ParameterId::MainPlayground2>,
                              MinimizedParameterEntry<Core::ParameterId::MainPlayground3>>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<Core::ParameterId::MainPlayground1, Color::Red, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Leftmost>,
                  MaximizedParameterEntry<Core::ParameterId::MainPlayground2, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Rightmost>,
                  MaximizedParameterEntry<Core::ParameterId::MainPlayground3, Color::Green, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthWest>,
                  MaximizedParameterEntry<Core::ParameterId::MainPlayground4, Color::Purple,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthEast>,
                  MaximizedParameterEntry<Core::ParameterId::MainPlayground5, Color::White, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<Core::ParameterId::MainPlayground6, Color::Red, UiEvent::ReleasedKnobRotate,
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
        = Entries<MaximizedParameterEntry<Core::ParameterId::Playground1, Color::Red, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Leftmost>,
                  MaximizedParameterEntry<Core::ParameterId::Playground2, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Rightmost>,
                  MaximizedParameterEntry<Core::ParameterId::Playground3, Color::Green, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthWest>,
                  MaximizedParameterEntry<Core::ParameterId::Playground4, Color::Purple, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthEast>,
                  MaximizedParameterEntry<Core::ParameterId::Playground5, Color::White, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<Core::ParameterId::Playground6, Color::Red, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::SouthWest>,
                  MaximizedParameterEntry<Core::ParameterId::Playground7, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::SouthEast>>;
    using MaximizedCustom = Entries<>;
  };

  template <> struct ToolboxDefinition<Toolbox::Steps>
  {
    constexpr static auto title = "Step Wizard";
    using Minimized = Entries<>;
    using MaximizedParameters = Entries<>;

    struct OneFitsAll
    {
      static constexpr auto title = "One Fits All";
    };

    struct Rotate
    {
      static constexpr auto title = "Rotate";
    };

    struct Steps
    {
      static constexpr auto title = "Steps";
    };

    struct Gaps
    {
      static constexpr auto title = "Gaps";
    };

    struct All
    {
      static constexpr auto title = "All";
    };

    struct None
    {
      static constexpr auto title = "None";
    };

    struct Mirror
    {
      static constexpr auto title = "Mirror";
    };

    struct Invert
    {
      static constexpr auto title = "Invert";
    };

    using MaximizedCustom = Entries<
        MaximizedCustomEntry<OneFitsAll, Color::Red, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Leftmost>,
        MaximizedCustomEntry<Rotate, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Center>,
        MaximizedCustomEntry<Steps, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::SouthWest>,
        MaximizedCustomEntry<Gaps, Color::Purple, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::SouthEast>,
        MaximizedCustomEntry<All, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_NorthEast>,
        MaximizedCustomEntry<None, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
        MaximizedCustomEntry<Invert, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthEast>,
        MaximizedCustomEntry<Mirror, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthWest>

        >;
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
                  MaximizedParameterEntry<Core::ParameterId::Speed, Color::Red, UiEvent::KnobClick, UiAction::Default,
                                          Knob::Rightmost>,
                  MaximizedParameterEntry<Core::ParameterId::Balance, Color::Purple, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Leftmost>,
                  MaximizedParameterEntry<Core::ParameterId::Shuffle, Color::White, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthEast>,
                  MaximizedParameterEntry<Core::ParameterId::Reverse, Color::Blue, UiEvent::ButtonPress,
                                          UiAction::Toggle, SoftButton::Left_Center>,
                  MaximizedParameterEntry<Core::ParameterId::Mute, Color::Green, UiEvent::ButtonPress, UiAction::Toggle,
                                          SoftButton::Left_South>>;

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
        MaximizedCustomEntry<Up, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
        MaximizedCustomEntry<Down, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_South>,
        MaximizedCustomEntry<Leave, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_West>,
        MaximizedCustomEntry<Enter, Color::Purple, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_East>,
        MaximizedCustomEntry<Load, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
        MaximizedCustomEntry<Prelisten, Color::Purple, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Right_SouthEast>>;
  };

  template <> struct ToolboxDefinition<Toolbox::Waveform>
  {
    constexpr static auto title = "Waveform";
    using Minimized = Entries<>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<Core::ParameterId::EnvelopeFadeInPos, Color::Red, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDecZoomed, Knob::SouthWest>,
                  MaximizedParameterEntry<Core::ParameterId::EnvelopeFadedInPos, Color::Blue,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::NorthWest>,
                  MaximizedParameterEntry<Core::ParameterId::EnvelopeFadeOutPos, Color::Green,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::NorthEast>,
                  MaximizedParameterEntry<Core::ParameterId::EnvelopeFadedOutPos, Color::Purple,
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
        MaximizedCustomEntry<Zoom, Color::Red, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Leftmost>,
        MaximizedCustomEntry<Scroll, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Rightmost>,
        MaximizedCustomEntry<HitPoint, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Center>>;
  };

  template <> struct ToolboxDefinition<Toolbox::Mute>
  {
    constexpr static auto title = "Mute";
    using Minimized = Entries<>;
    using MaximizedParameters = Entries<>;

    struct SaveArmed
    {
      static constexpr auto title = "Save";
    };

    struct SaveUnarmed
    {
      static constexpr auto title = "Save";
    };

    struct Slot1
    {
      static constexpr auto title = "Slot 1";
    };

    struct Slot2
    {
      static constexpr auto title = "Slot 2";
    };

    struct Slot3
    {
      static constexpr auto title = "Slot 3";
    };

    struct Slot4
    {
      static constexpr auto title = "Slot 4";
    };

    struct Slot5
    {
      static constexpr auto title = "Slot 5";
    };

    struct Slot6
    {
      static constexpr auto title = "Slot 6";
    };

    struct UnmuteAll
    {
      static constexpr auto title = "Unmute All";
    };

    struct LastMute
    {
      static constexpr auto title = "Last Mute";
    };

    using MaximizedCustom = Entries<
        MaximizedCustomEntry<UnmuteAll, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_SouthWest>,
        MaximizedCustomEntry<LastMute, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_South>,
        MaximizedCustomEntry<SaveArmed, Color::Purple, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
        MaximizedCustomEntry<SaveUnarmed, Color::Red, UiEvent::ButtonRelease, UiAction::Invoke,
                             SoftButton::Right_North>,
        MaximizedCustomEntry<Slot1, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_West>,
        MaximizedCustomEntry<Slot2, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
        MaximizedCustomEntry<Slot3, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_East>,
        MaximizedCustomEntry<Slot4, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthWest>,
        MaximizedCustomEntry<Slot5, Color::Purple, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_South>,
        MaximizedCustomEntry<Slot6, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthEast>>;
  };

  template <> struct ToolboxDefinition<Toolbox::MixerChannel>
  {
    constexpr static auto title = "Mixer Channel";
    using Minimized = Entries<MinimizedParameterEntry<Core::ParameterId::ChannelOnOff>,
                              MinimizedParameterEntry<Core::ParameterId::ChannelVolume>>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<Core::ParameterId::ChannelOnOff, Color::Red, UiEvent::ButtonPress,
                                          UiAction::Toggle, SoftButton::Right_Center>,
                  MaximizedParameterEntry<Core::ParameterId::ChannelVolume, Color::Red, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<Core::ParameterId::ChannelDelaySend, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthWest>,
                  MaximizedParameterEntry<Core::ParameterId::ChannelReverbSend, Color::Green,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthEast>,
                  MaximizedParameterEntry<Core::ParameterId::ChannelDelayPrePost, Color::Blue, UiEvent::ButtonPress,
                                          UiAction::Toggle, SoftButton::Left_North>,
                  MaximizedParameterEntry<Core::ParameterId::ChannelReverbPrePost, Color::Green, UiEvent::ButtonPress,
                                          UiAction::Toggle, SoftButton::Right_North>>;
    using MaximizedCustom = Entries<>;
  };

  template <> struct ToolboxDefinition<Toolbox::Reverb>
  {
    constexpr static auto title = "Reverb";
    using Minimized = Entries<MinimizedParameterEntry<Core::ParameterId::GlobalReverbRoomSize>,
                              MinimizedParameterEntry<Core::ParameterId::GlobalReverbColor>>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<Core::ParameterId::GlobalReverbRoomSize, Color::Blue,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::SouthWest>,
                  MaximizedParameterEntry<Core::ParameterId::GlobalReverbPreDelay, Color::Green,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthWest>,
                  MaximizedParameterEntry<Core::ParameterId::GlobalReverbColor, Color::Purple,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::SouthEast>,
                  MaximizedParameterEntry<Core::ParameterId::GlobalReverbChorus, Color::Red,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthEast>,
                  MaximizedParameterEntry<Core::ParameterId::GlobalReverbReturn, Color::White,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<Core::ParameterId::GlobalReverbOnOff, Color::Red, UiEvent::ButtonPress,
                                          UiAction::Toggle, SoftButton::Right_Center>>;
    using MaximizedCustom = Entries<>;
  };

  template <> struct ToolboxDefinition<Toolbox::ColorAdjust>
  {
    struct Led_R
    {
      static constexpr auto title = "LED Red";
    };

    struct Led_G
    {
      static constexpr auto title = "LED Green";
    };

    struct Led_B
    {
      static constexpr auto title = "LED Blue";
    };

    struct Screen_R
    {
      static constexpr auto title = "Screen Red";
    };

    struct Screen_G
    {
      static constexpr auto title = "Screen Green";
    };

    struct Screen_B
    {
      static constexpr auto title = "Screen Blue";
    };

    constexpr static auto title = "Color Adjustment";
    using Minimized = Entries<>;
    using MaximizedParameters = Entries<>;
    using MaximizedCustom = Entries<
        MaximizedCustomEntry<Led_R, Color::Red, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Leftmost>,
        MaximizedCustomEntry<Led_G, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::NorthWest>,
        MaximizedCustomEntry<Led_B, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::SouthWest>,

        MaximizedCustomEntry<Screen_R, Color::Red, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Rightmost>,
        MaximizedCustomEntry<Screen_G, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::NorthEast>,
        MaximizedCustomEntry<Screen_B, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::SouthEast>

        >;
  };
}
