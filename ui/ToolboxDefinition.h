#pragma once

#include <ui/Types.h>
#include <ui/ParameterDescriptor.h>

namespace Ui
{
  using ParameterId = Core::ParameterId;

  template <Toolbox t> struct ToolboxDefinition;

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

  template <typename I, Color C, UiEvent E, UiAction A, auto P> struct Entry
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

    static auto cat(auto a, auto b)
    {
      using C = decltype(std::tuple_cat(a, b));
      return std::apply([](auto... a) { return Entries<decltype(a)...> {}; }, C {});
    }

    using Tuple = std::tuple<E...>;
    template <typename F> using Add = decltype(cat(std::tuple<E...> {}, typename F::Tuple {}));
  };

  struct PreviousToolbox
  {
    constexpr static auto title = "Prev";
  };

  struct NextToolbox
  {
    constexpr static auto title = "Next";
  };

  struct GotoToolboxTile
  {
    constexpr static auto title = "Tile";
  };
  struct GotoToolboxWaveform
  {
    constexpr static auto title = "Waveform";
  };
  struct GotoToolboxGlobal
  {
    constexpr static auto title = "Global";
  };
  struct GotoToolboxMute
  {
    constexpr static auto title = "Mute";
  };
  struct GotoToolboxSteps
  {
    constexpr static auto title = "Steps";
  };
  struct GotoToolboxSnapshots
  {
    constexpr static auto title = "Snapshots";
  };
  struct GotoToolboxReverb
  {
    constexpr static auto title = "Reverb";
  };

  using ToolboxNavigation = Entries<
      Entry<PreviousToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_NorthWest>,
      Entry<NextToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>,
      Entry<GotoToolboxTile, Color::Orange, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_West>,
      Entry<GotoToolboxWaveform, Color::LightBlue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_Center>,
      Entry<GotoToolboxSteps, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_East>,
      Entry<GotoToolboxMute, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthWest>,
      Entry<GotoToolboxReverb, Color::Yellow, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_South>,
      Entry<GotoToolboxSnapshots, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthEast>>;

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

    using Entires
        = Entries<Entry<ParameterDescriptor<ParameterId::GlobalVolume>, Color::Green, UiEvent::ReleasedKnobRotate,
                        UiAction::IncDec, Knob::Center>,
                  Entry<ParameterDescriptor<ParameterId::GlobalPrelistenVolume>, Color::Green,
                        UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthEast>,
                  Entry<ParameterDescriptor<ParameterId::GlobalTempo>, Color::Blue, UiEvent::ReleasedKnobRotate,
                        UiAction::IncDec, Knob::Rightmost>,

                  Entry<TapNSync, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
                  Entry<StartSlowDown, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
                  Entry<StopSlowDown, Color::Blue, UiEvent::ButtonRelease, UiAction::Invoke, SoftButton::Right_North>,
                  Entry<StartSpeedUp, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_NorthEast>,
                  Entry<StopSpeedUp, Color::Blue, UiEvent::ButtonRelease, UiAction::Invoke,
                        SoftButton::Right_NorthEast>>::Add<ToolboxNavigation>;
  };

  template <> struct ToolboxDefinition<Toolbox::Steps>
  {
    constexpr static auto title = "Step Wizard";

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
    struct ArmSkipStep
    {
      static constexpr auto title = "Skip";
    };
    struct DisarmSkipStep
    {
      static constexpr auto title = "Skip";
    };
    struct ArmTriplet
    {
      static constexpr auto title = "Triplet";
    };
    struct DisarmTriplet
    {
      static constexpr auto title = "Triplet";
    };

    using Entires
        = Entries<Entry<OneFitsAll, Color::Red, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Leftmost>,
                  Entry<Rotate, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Center>,
                  Entry<Steps, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::SouthWest>,
                  Entry<Gaps, Color::Magenta, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::SouthEast>,
                  Entry<All, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_NorthEast>,
                  Entry<None, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
                  Entry<Invert, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthEast>,
                  Entry<Mirror, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthWest>,
                  Entry<ArmSkipStep, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_East>,
                  Entry<DisarmSkipStep, Color::Red, UiEvent::ButtonRelease, UiAction::Invoke, SoftButton::Right_East>,
                  Entry<ArmTriplet, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_West>,
                  Entry<DisarmTriplet, Color::Red, UiEvent::ButtonRelease, UiAction::Invoke,
                        SoftButton::Right_West>>::Add<ToolboxNavigation>;
  };

  template <> struct ToolboxDefinition<Toolbox::Tile>
  {
    constexpr static auto title = "Tile";

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

    using Entires = Entries<
        Entry<ParameterDescriptor<ParameterId::Gain>, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::IncDec,
              Knob::Center>,
        Entry<ParameterDescriptor<ParameterId::Speed>, Color::Magenta, UiEvent::ReleasedKnobRotate, UiAction::IncDec,
              Knob::SouthEast>,
        Entry<ParameterDescriptor<ParameterId::Balance>, Color::Magenta, UiEvent::ReleasedKnobRotate, UiAction::IncDec,
              Knob::Leftmost>,
        Entry<ParameterDescriptor<ParameterId::Shuffle>, Color::Magenta, UiEvent::ReleasedKnobRotate, UiAction::IncDec,
              Knob::NorthWest>,
        Entry<ParameterDescriptor<ParameterId::ReverbSend>, Color::Magenta, UiEvent::ReleasedKnobRotate,
              UiAction::IncDec, Knob::SouthWest>,
        Entry<ParameterDescriptor<ParameterId::Reverse>, Color::LightGreen, UiEvent::ButtonPress, UiAction::Toggle,
              SoftButton::Right_West>,
        Entry<ParameterDescriptor<ParameterId::GlobalPrelistenVolume>, Color::Green, UiEvent::ReleasedKnobRotate,
              UiAction::IncDec, Knob::NorthEast>,
        Entry<ParameterDescriptor<ParameterId::Mute>, Color::Red, UiEvent::ButtonPress, UiAction::Toggle,
              SoftButton::Right_East>,
        Entry<Up, Color::Yellow, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
        Entry<Down, Color::Yellow, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_South>,
        Entry<Leave, Color::Yellow, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthWest>,
        Entry<Enter, Color::Yellow, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthEast>,
        Entry<Load, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
        Entry<IncDec, Color::Yellow, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Rightmost>,
        Entry<Load, Color::Yellow, UiEvent::KnobClick, UiAction::Invoke, Knob::Rightmost>>::Add<ToolboxNavigation>;
  };

  template <> struct ToolboxDefinition<Toolbox::Waveform>
  {
    constexpr static auto title = "Waveform";

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

    using Entires = Entries<Entry<ParameterDescriptor<ParameterId::EnvelopeFadeInPos>, Color::Red,
                                  UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::SouthWest>,
                            Entry<ParameterDescriptor<ParameterId::EnvelopeFadedInPos>, Color::Blue,
                                  UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::NorthWest>,
                            Entry<ParameterDescriptor<ParameterId::EnvelopeFadeOutPos>, Color::Green,
                                  UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::NorthEast>,
                            Entry<ParameterDescriptor<ParameterId::EnvelopeFadedOutPos>, Color::Magenta,
                                  UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::SouthEast>,
                            Entry<ParameterDescriptor<ParameterId::Gain>, Color::Green, UiEvent::ReleasedKnobRotate,
                                  UiAction::IncDec, Knob::Center>,
                            Entry<Zoom, Color::Red, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Leftmost>,
                            Entry<HitPoint, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::Invoke,
                                  Knob::Rightmost>>::Add<ToolboxNavigation>;
  };

  template <> struct ToolboxDefinition<Toolbox::Mute>
  {
    constexpr static auto title = "Mute";

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

    using Entires
        = Entries<Entry<UnmuteAll, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_NorthEast>,
                  Entry<LastMute, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_East>,
                  Entry<SaveArmed, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
                  Entry<SaveUnarmed, Color::White, UiEvent::ButtonRelease, UiAction::Invoke, SoftButton::Right_North>,
                  Entry<Slot1, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_West>,
                  Entry<Slot2, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
                  //Entry<Slot3, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_East>,
                  Entry<Slot4, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthWest>,
                  Entry<Slot5, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_South>,
                  Entry<Slot6, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                        SoftButton::Right_SouthEast>>::Add<ToolboxNavigation>;
  };

  template <> struct ToolboxDefinition<Toolbox::Reverb>
  {
    constexpr static auto title = "Reverb";

    using Entires = Entries<Entry<ParameterDescriptor<ParameterId::GlobalReverbRoomSize>, Color::Blue,
                                  UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::SouthWest>,
                            Entry<ParameterDescriptor<ParameterId::GlobalReverbPreDelay>, Color::Green,
                                  UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthWest>,
                            Entry<ParameterDescriptor<ParameterId::GlobalReverbColor>, Color::Magenta,
                                  UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::SouthEast>,
                            Entry<ParameterDescriptor<ParameterId::GlobalReverbChorus>, Color::Red,
                                  UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthEast>,
                            Entry<ParameterDescriptor<ParameterId::GlobalReverbReturn>, Color::White,
                                  UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::Center>,
                            Entry<ParameterDescriptor<ParameterId::GlobalReverbOnOff>, Color::Red, UiEvent::ButtonPress,
                                  UiAction::Toggle, SoftButton::Right_Center>>::Add<ToolboxNavigation>;
  };

  template <> struct ToolboxDefinition<Toolbox::Snapshots>
  {
    constexpr static auto title = "Snapshots";

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

    using Entires
        = Entries<Entry<SaveArmed, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
                  Entry<SaveUnarmed, Color::Red, UiEvent::ButtonRelease, UiAction::Invoke, SoftButton::Right_North>,
                  Entry<Slot1, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_West>,
                  Entry<Slot2, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
                  Entry<Slot3, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_East>,
                  Entry<Slot4, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthWest>,
                  Entry<Slot5, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_South>,
                  Entry<Slot6, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke,
                        SoftButton::Right_SouthEast>>::Add<ToolboxNavigation>;
  };

  /*
  template <> struct ToolboxDefinition<Toolbox::MainPlayground>
  {
    constexpr static auto title = "Main Playground";
    using Minimized = Entries<MinimizedParameterEntry<ParameterId::MainPlayground1>,
                              MinimizedParameterEntry<ParameterId::MainPlayground2>,
                              MinimizedParameterEntry<ParameterId::MainPlayground3>>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<ParameterId::MainPlayground1, Color::Red, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Leftmost>,
                  MaximizedParameterEntry<ParameterId::MainPlayground2, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Rightmost>,
                  MaximizedParameterEntry<ParameterId::MainPlayground3, Color::Green, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthWest>,
                  MaximizedParameterEntry<ParameterId::MainPlayground4, Color::Magenta,
                                          UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthEast>,
                  MaximizedParameterEntry<ParameterId::MainPlayground5, Color::White, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<ParameterId::MainPlayground6, Color::Red, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::SouthWest>,
                  MaximizedParameterEntry<ParameterId::MainPlayground7, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::SouthEast>>;
    using Entires = Entries<
        Entry<PreviousToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_NorthWest>,
        Entry<NextToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>>;
  };

  template <> struct ToolboxDefinition<Toolbox::Playground>
  {
    constexpr static auto title = "Playground";
    using Minimized = Entries<MinimizedParameterEntry<ParameterId::Playground1>,
                              MinimizedParameterEntry<ParameterId::Playground2>,
                              MinimizedParameterEntry<ParameterId::Playground3>>;
    using MaximizedParameters
        = Entries<MaximizedParameterEntry<ParameterId::Playground1, Color::Red, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Leftmost>,
                  MaximizedParameterEntry<ParameterId::Playground2, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Rightmost>,
                  MaximizedParameterEntry<ParameterId::Playground3, Color::Green, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthWest>,
                  MaximizedParameterEntry<ParameterId::Playground4, Color::Magenta, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::NorthEast>,
                  MaximizedParameterEntry<ParameterId::Playground5, Color::White, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::Center>,
                  MaximizedParameterEntry<ParameterId::Playground6, Color::Red, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::SouthWest>,
                  MaximizedParameterEntry<ParameterId::Playground7, Color::Blue, UiEvent::ReleasedKnobRotate,
                                          UiAction::IncDec, Knob::SouthEast>>;
    using Entires = Entries<
        Entry<PreviousToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_NorthWest>,
        Entry<NextToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>>;
  };*/
  /*
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
    using Entires = Entries<
        Entry<Led_R, Color::Red, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Leftmost>,
        Entry<Led_G, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::NorthWest>,
        Entry<Led_B, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::SouthWest>,

        Entry<Screen_R, Color::Red, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Rightmost>,
        Entry<Screen_G, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::NorthEast>,
        Entry<Screen_B, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::SouthEast>,

        Entry<PreviousToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke,
                             SoftButton::Left_NorthWest>,
        Entry<NextToolbox, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>>;
  };*/
}
