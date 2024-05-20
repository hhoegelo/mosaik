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

  struct PreviousToolbox
  {
    constexpr static auto title = "Prev";
  };

  struct NextToolbox
  {
    constexpr static auto title = "Next";
  };

  template <> struct ToolboxDefinition<Toolbox::Global>
  {
    constexpr static auto title = "Global";

    struct TapNSync
    {
      constexpr static auto title = "Tap'n Sync";
    };

    struct StartSlowDown
    {
      constexpr static auto title = "Slow Down";
    };

    struct StopSlowDown
    {
      constexpr static auto title = "Slow Down";
    };

    struct StartSpeedUp
    {
      constexpr static auto title = "Speed Up";
    };

    struct StopSpeedUp
    {
      constexpr static auto title = "Speed Up";
    };

    using Minimized = Entries<MinimizedParameterEntry<Core::ParameterId::GlobalVolume>,
                              MinimizedParameterEntry<Core::ParameterId::GlobalTempo>>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<Core::ParameterId::GlobalVolume, Color::Green, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<Core::ParameterId::GlobalPrelistenVolume, Color::Purple,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::Rightmost>,
                  MaximizedParameterEntry<Core::ParameterId::GlobalTempo, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Leftmost>>;
    using MaximizedCustom = Entries<
        MaximizedCustomEntry<PreviousToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>,
        MaximizedCustomEntry<TapNSync, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
        MaximizedCustomEntry<StartSlowDown, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Right_SouthWest>,
        MaximizedCustomEntry<StopSlowDown, Color::Red, UiEvent::ButtonRelease, UiAction::Invoke,
                             SoftButton::Right_SouthWest>,
        MaximizedCustomEntry<StartSpeedUp, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Right_SouthEast>,
        MaximizedCustomEntry<StopSpeedUp, Color::Red, UiEvent::ButtonRelease, UiAction::Invoke,
                             SoftButton::Right_SouthEast>>;
  };
  /*
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
    using MaximizedCustom = Entries<
        MaximizedCustomEntry<PreviousToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>>;
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
    using MaximizedCustom = Entries<
        MaximizedCustomEntry<PreviousToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>>;
  };*/

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
        MaximizedCustomEntry<Mirror, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthWest>,

        MaximizedCustomEntry<PreviousToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>>;
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
                  MaximizedParameterEntry<Core::ParameterId::Speed, Color::Purple, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Rightmost>,
                  MaximizedParameterEntry<Core::ParameterId::Speed, Color::Purple, UiEvent::KnobClick,
                                          UiAction::Default, Knob::Rightmost>,
                  MaximizedParameterEntry<Core::ParameterId::Balance, Color::Purple, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Leftmost>,
                  MaximizedParameterEntry<Core::ParameterId::Shuffle, Color::Purple, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthEast>,
                  MaximizedParameterEntry<Core::ParameterId::Reverse, Color::Purple, UiEvent::ButtonPress,
                                          UiAction::Toggle, SoftButton::Left_Center>,
                  MaximizedParameterEntry<Core::ParameterId::Mute, Color::Red, UiEvent::ButtonPress, UiAction::Toggle,
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

    struct IncDec
    {
      static constexpr auto title = "Inc/Dec";
    };

    using MaximizedCustom = Entries<
        MaximizedCustomEntry<Up, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
        MaximizedCustomEntry<Down, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_South>,
        MaximizedCustomEntry<Leave, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthWest>,
        MaximizedCustomEntry<Enter, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthEast>,
        MaximizedCustomEntry<Load, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,

        MaximizedCustomEntry<PreviousToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>,
        MaximizedCustomEntry<IncDec, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::SouthEast>,
        MaximizedCustomEntry<Load, Color::Blue, UiEvent::KnobClick, UiAction::Invoke, Knob::SouthEast>>;
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
      constexpr static float acceleration = 10.f;
    };

    struct HitPoint
    {
      static constexpr auto title = "Hit Point";
      constexpr static float acceleration = 10.f;
    };

    using MaximizedCustom = Entries<
        MaximizedCustomEntry<Zoom, Color::Red, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Leftmost>,
        MaximizedCustomEntry<Scroll, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Rightmost>,
        MaximizedCustomEntry<HitPoint, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Center>,

        MaximizedCustomEntry<PreviousToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>>;
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
        MaximizedCustomEntry<Slot6, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthEast>,

        MaximizedCustomEntry<PreviousToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>>;
  };

  template <> struct ToolboxDefinition<Toolbox::MixerChannel>
  {
    constexpr static auto title = "Mixer Channel";
    using Minimized = Entries<MinimizedParameterEntry<Core::ParameterId::ChannelOnOff>,
                              MinimizedParameterEntry<Core::ParameterId::ChannelVolume>>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<Core::ParameterId::ChannelOnOff, Color::Red, UiEvent::ButtonPress,
                                          UiAction::Toggle, SoftButton::Right_South>,
                  MaximizedParameterEntry<Core::ParameterId::ChannelVolume, Color::Red, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<Core::ParameterId::ChannelDelaySend, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthWest>,
                  MaximizedParameterEntry<Core::ParameterId::ChannelReverbSend, Color::Green,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthEast>,
                  MaximizedParameterEntry<Core::ParameterId::ChannelDelayPrePost, Color::Blue, UiEvent::ButtonPress,
                                          UiAction::Toggle, SoftButton::Left_Center>,
                  MaximizedParameterEntry<Core::ParameterId::ChannelReverbPrePost, Color::Green, UiEvent::ButtonPress,
                                          UiAction::Toggle, SoftButton::Right_Center>>;
    using MaximizedCustom = Entries<
        MaximizedCustomEntry<PreviousToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>>;
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
    using MaximizedCustom = Entries<
        MaximizedCustomEntry<PreviousToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>>;
  };

  template <> struct ToolboxDefinition<Toolbox::ColorAdjust>
  {
    struct Led_R
    {
      constexpr static auto title = "LED Red";
      constexpr static float acceleration = 10.f;
    };

    struct Led_G
    {
      constexpr static auto title = "LED Green";
      constexpr static float acceleration = 10.f;
    };

    struct Led_B
    {
      constexpr static auto title = "LED Blue";
      constexpr static float acceleration = 10.f;
    };

    struct Screen_R
    {
      constexpr static auto title = "Screen Red";
      constexpr static float acceleration = 10.f;
    };

    struct Screen_G
    {
      constexpr static auto title = "Screen Green";
      constexpr static float acceleration = 10.f;
    };

    struct Screen_B
    {
      constexpr static auto title = "Screen Blue";
      constexpr static float acceleration = 10.f;
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
        MaximizedCustomEntry<Screen_B, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::SouthEast>,

        MaximizedCustomEntry<PreviousToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>>;
  };
}
