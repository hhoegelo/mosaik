#pragma once

#include <ui/Types.h>
#include <ui/ParameterDescriptor.h>

namespace Ui
{

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


  struct GotoToolboxTile { constexpr static auto title = "Tile"; };
  struct GotoToolboxWaveform { constexpr static auto title = "Waveform"; };
  struct GotoToolboxGlobal { constexpr static auto title = "Global"; };
  struct GotoToolboxMute { constexpr static auto title = "Mute"; };
  struct GotoToolboxSteps { constexpr static auto title = "Steps"; };
  struct GotoToolboxSnapshots { constexpr static auto title = "Snapshots"; };
  struct GotoToolboxReverb { constexpr static auto title = "Reverb"; };


  template <> struct ToolboxDefinition<Toolbox::Global>
  {
    constexpr static auto title = "Global";

    struct TapNSync { constexpr static auto title = "Tap'n Sync"; };
    struct StartSlowDown { constexpr static auto title = "Slow Down"; };
    struct StopSlowDown { constexpr static auto title = "Slow Down"; };
    struct StartSpeedUp { constexpr static auto title = "Speed Up"; };
    struct StopSpeedUp { constexpr static auto title = "Speed Up"; };

    using MaximizedParameters = Entries<
        MaximizedParameterEntry<Core::ParameterId::GlobalVolume, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::Center>,
        MaximizedParameterEntry<Core::ParameterId::GlobalPrelistenVolume, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthEast>,
        MaximizedParameterEntry<Core::ParameterId::GlobalTempo, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::Rightmost>
    >;
    using MaximizedCustom = Entries<
        MaximizedCustomEntry<TapNSync, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
        MaximizedCustomEntry<StartSlowDown, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
        MaximizedCustomEntry<StopSlowDown, Color::Blue, UiEvent::ButtonRelease, UiAction::Invoke, SoftButton::Right_North>,
        MaximizedCustomEntry<StartSpeedUp, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_NorthEast>,
        MaximizedCustomEntry<StopSpeedUp, Color::Blue, UiEvent::ButtonRelease, UiAction::Invoke, SoftButton::Right_NorthEast>,

        MaximizedCustomEntry<PreviousToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>,
        MaximizedCustomEntry<GotoToolboxTile, Color::Orange, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_West>,
        MaximizedCustomEntry<GotoToolboxWaveform, Color::LightBlue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_Center>,
        MaximizedCustomEntry<GotoToolboxSteps, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_East>,
        MaximizedCustomEntry<GotoToolboxMute, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthWest>,
        MaximizedCustomEntry<GotoToolboxReverb, Color::Yellow, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_South>,
        MaximizedCustomEntry<GotoToolboxSnapshots, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthEast>
    >;
  };


  template <> struct ToolboxDefinition<Toolbox::Steps>
  {
    constexpr static auto title = "Step Wizard";
    using Minimized = Entries<>;
    using MaximizedParameters = Entries<>;

    struct OneFitsAll { static constexpr auto title = "One Fits All"; };
    struct Rotate { static constexpr auto title = "Rotate"; };
    struct Steps { static constexpr auto title = "Steps"; };
    struct Gaps { static constexpr auto title = "Gaps"; };
    struct All { static constexpr auto title = "All"; };
    struct None { static constexpr auto title = "None"; };
    struct Mirror { static constexpr auto title = "Mirror"; };
    struct Invert { static constexpr auto title = "Invert"; };
    struct ArmSkipStep { static constexpr auto title = "Skip"; };
    struct DisarmSkipStep { static constexpr auto title = "Skip"; };
    struct ArmTriplet { static constexpr auto title = "Triplet"; };
    struct DisarmTriplet { static constexpr auto title = "Triplet"; };

    using MaximizedCustom = Entries<
        MaximizedCustomEntry<OneFitsAll, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Leftmost>,
        MaximizedCustomEntry<Rotate, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Center>,
        MaximizedCustomEntry<Steps, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::SouthWest>,
        MaximizedCustomEntry<Gaps, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::SouthEast>,

        MaximizedCustomEntry<All, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_NorthEast>,
        MaximizedCustomEntry<None, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
        MaximizedCustomEntry<Invert, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthEast>,
        MaximizedCustomEntry<Mirror, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthWest>,
        MaximizedCustomEntry<ArmSkipStep, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_East>,
        MaximizedCustomEntry<DisarmSkipStep, Color::Red, UiEvent::ButtonRelease, UiAction::Invoke, SoftButton::Right_East>,
        MaximizedCustomEntry<ArmTriplet, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_West>,
        MaximizedCustomEntry<DisarmTriplet, Color::Red, UiEvent::ButtonRelease, UiAction::Invoke, SoftButton::Right_West>,

        MaximizedCustomEntry<PreviousToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>,
        MaximizedCustomEntry<GotoToolboxTile, Color::Orange, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_West>,
        MaximizedCustomEntry<GotoToolboxWaveform, Color::LightBlue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_Center>,
        MaximizedCustomEntry<GotoToolboxSteps, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_East>,
        MaximizedCustomEntry<GotoToolboxMute, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthWest>,
        MaximizedCustomEntry<GotoToolboxReverb, Color::Yellow, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_South>,
        MaximizedCustomEntry<GotoToolboxSnapshots, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthEast>
        >;
  };


  template <> struct ToolboxDefinition<Toolbox::Tile>
  {
    constexpr static auto title = "Tile";

    struct Up { static constexpr auto title = "Up"; };
    struct Down { static constexpr auto title = "Down"; };
    struct Enter { static constexpr auto title = "Enter"; };
    struct Leave { static constexpr auto title = "Leave"; };
    struct Load { static constexpr auto title = "Load"; };
    struct IncDec { static constexpr auto title = "Inc/Dec"; };

    using MaximizedParameters = Entries<
        MaximizedParameterEntry<Core::ParameterId::Gain, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::Center>,
        MaximizedParameterEntry<Core::ParameterId::Speed, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::SouthEast>,
        MaximizedParameterEntry<Core::ParameterId::Balance, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::Leftmost>,
        MaximizedParameterEntry<Core::ParameterId::Shuffle, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::SouthWest>,
        MaximizedParameterEntry<Core::ParameterId::ReverbSend, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthWest>,
        MaximizedParameterEntry<Core::ParameterId::Reverse, Color::Blue, UiEvent::ButtonPress, UiAction::Toggle, SoftButton::Right_West>,
        MaximizedParameterEntry<Core::ParameterId::GlobalPrelistenVolume, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthEast>,
        MaximizedParameterEntry<Core::ParameterId::Mute, Color::Red, UiEvent::ButtonPress, UiAction::Toggle, SoftButton::Right_East>
    >;

    using MaximizedCustom = Entries<
        MaximizedCustomEntry<Up, Color::Orange, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
        MaximizedCustomEntry<Down, Color::Orange, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_South>,
        MaximizedCustomEntry<Leave, Color::Orange, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthWest>,
        MaximizedCustomEntry<Enter, Color::Orange, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthEast>,
        MaximizedCustomEntry<Load, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
        MaximizedCustomEntry<IncDec, Color::Yellow, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Rightmost>,
        MaximizedCustomEntry<Load, Color::White, UiEvent::KnobClick, UiAction::Invoke, Knob::Rightmost>,

        MaximizedCustomEntry<PreviousToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>,
        MaximizedCustomEntry<GotoToolboxTile, Color::Orange, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_West>,
        MaximizedCustomEntry<GotoToolboxWaveform, Color::LightBlue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_Center>,
        MaximizedCustomEntry<GotoToolboxSteps, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_East>,
        MaximizedCustomEntry<GotoToolboxMute, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthWest>,
        MaximizedCustomEntry<GotoToolboxReverb, Color::Yellow, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_South>,
        MaximizedCustomEntry<GotoToolboxSnapshots, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthEast>
    >;
  };

  template <> struct ToolboxDefinition<Toolbox::Waveform>
  {
    constexpr static auto title = "Waveform";
    using Minimized = Entries<>;

    struct Zoom { static constexpr auto title = "Zoom"; };
    struct Scroll { static constexpr auto title = "Scroll"; constexpr static float acceleration = 10.f; };
    struct HitPoint { static constexpr auto title = "Hit Point"; constexpr static float acceleration = 10.f; };
    struct Move { static constexpr auto title = "Move"; constexpr static float acceleration = 10.f; };

    using MaximizedParameters = Entries<
        MaximizedParameterEntry<Core::ParameterId::EnvelopeFadeInPos, Color::LightBlue, UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::SouthWest>,
        MaximizedParameterEntry<Core::ParameterId::EnvelopeFadedInPos, Color::LightBlue, UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::NorthWest>,
        MaximizedParameterEntry<Core::ParameterId::EnvelopeFadeOutPos, Color::LightBlue, UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::NorthEast>,
        MaximizedParameterEntry<Core::ParameterId::EnvelopeFadedOutPos, Color::LightBlue, UiEvent::ReleasedKnobRotate, UiAction::IncDecZoomed, Knob::SouthEast>,
        MaximizedParameterEntry<Core::ParameterId::Gain, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::Rightmost>
    >;

    using MaximizedCustom = Entries<
        MaximizedCustomEntry<Zoom, Color::Red, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Leftmost>,
        //MaximizedCustomEntry<Scroll, Color::Blue, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Rightmost>,
        //MaximizedCustomEntry<Move, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, Knob::SouthEast>,
        MaximizedCustomEntry<HitPoint, Color::Red, UiEvent::ReleasedKnobRotate, UiAction::Invoke, Knob::Center>,


        MaximizedCustomEntry<PreviousToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>,
        MaximizedCustomEntry<GotoToolboxTile, Color::Orange, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_West>,
        MaximizedCustomEntry<GotoToolboxWaveform, Color::LightBlue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_Center>,
        MaximizedCustomEntry<GotoToolboxSteps, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_East>,
        MaximizedCustomEntry<GotoToolboxMute, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthWest>,
        MaximizedCustomEntry<GotoToolboxReverb, Color::Yellow, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_South>,
        MaximizedCustomEntry<GotoToolboxSnapshots, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthEast>
    >;
  };

  template <> struct ToolboxDefinition<Toolbox::Mute>
  {
    constexpr static auto title = "Mute";
    using Minimized = Entries<>;
    using MaximizedParameters = Entries<>;

    struct SaveArmed { static constexpr auto title = "Save"; };
    struct SaveUnarmed { static constexpr auto title = "Save"; };
    struct Slot1 { static constexpr auto title = "Slot 1"; };
    struct Slot2 { static constexpr auto title = "Slot 2"; };
    struct Slot3 { static constexpr auto title = "Slot 3"; };
    struct Slot4 { static constexpr auto title = "Slot 4"; };
    struct Slot5 { static constexpr auto title = "Slot 5"; };
    struct Slot6 { static constexpr auto title = "Slot 6"; };
    struct UnmuteAll { static constexpr auto title = "Unmute All"; };
    struct LastMute { static constexpr auto title = "Last Mute"; };

    using MaximizedCustom = Entries<
        MaximizedCustomEntry<UnmuteAll, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_NorthEast>,
        MaximizedCustomEntry<LastMute, Color::Blue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_East>,
        MaximizedCustomEntry<SaveArmed, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
        MaximizedCustomEntry<SaveUnarmed, Color::White, UiEvent::ButtonRelease, UiAction::Invoke, SoftButton::Right_North>,
        MaximizedCustomEntry<Slot1, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_West>,
        MaximizedCustomEntry<Slot2, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
        //MaximizedCustomEntry<Slot3, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_East>,
        MaximizedCustomEntry<Slot4, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthWest>,
        MaximizedCustomEntry<Slot5, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_South>,
        MaximizedCustomEntry<Slot6, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthEast>,

        MaximizedCustomEntry<PreviousToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>,
        MaximizedCustomEntry<GotoToolboxTile, Color::Orange, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_West>,
        MaximizedCustomEntry<GotoToolboxWaveform, Color::LightBlue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_Center>,
        MaximizedCustomEntry<GotoToolboxSteps, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_East>,
        MaximizedCustomEntry<GotoToolboxMute, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthWest>,
        MaximizedCustomEntry<GotoToolboxReverb, Color::Yellow, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_South>,
        MaximizedCustomEntry<GotoToolboxSnapshots, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthEast>
    >;
  };


  template <> struct ToolboxDefinition<Toolbox::Reverb>
  {
    constexpr static auto title = "Reverb";

    using MaximizedParameters = Entries<
        MaximizedParameterEntry<Core::ParameterId::GlobalReverbRoomSize, Color::Yellow, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::SouthWest>,
        MaximizedParameterEntry<Core::ParameterId::GlobalReverbPreDelay, Color::Yellow, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthWest>,
        MaximizedParameterEntry<Core::ParameterId::GlobalReverbColor, Color::Yellow, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::SouthEast>,
        MaximizedParameterEntry<Core::ParameterId::GlobalReverbChorus, Color::Yellow, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::NorthEast>,
        MaximizedParameterEntry<Core::ParameterId::GlobalReverbReturn, Color::Green, UiEvent::ReleasedKnobRotate, UiAction::IncDec, Knob::Center>,
        MaximizedParameterEntry<Core::ParameterId::GlobalReverbOnOff, Color::Red, UiEvent::ButtonPress, UiAction::Toggle, SoftButton::Right_Center>
      >;
    using MaximizedCustom = Entries<
          MaximizedCustomEntry<PreviousToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_NorthWest>,
          MaximizedCustomEntry<NextToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>,
          MaximizedCustomEntry<GotoToolboxTile, Color::Orange, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_West>,
          MaximizedCustomEntry<GotoToolboxWaveform, Color::LightBlue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_Center>,
          MaximizedCustomEntry<GotoToolboxSteps, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_East>,
          MaximizedCustomEntry<GotoToolboxMute, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthWest>,
          MaximizedCustomEntry<GotoToolboxReverb, Color::Yellow, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_South>,
          MaximizedCustomEntry<GotoToolboxSnapshots, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthEast>
      >;
  };


  template <> struct ToolboxDefinition<Toolbox::Snapshots>
  {
    constexpr static auto title = "Snapshots";
    using Minimized = Entries<>;
    using MaximizedParameters = Entries<>;

    struct SaveArmed { static constexpr auto title = "Save"; };
    struct SaveUnarmed { static constexpr auto title = "Save"; };
    struct Slot1 { static constexpr auto title = "Slot 1"; };
    struct Slot2 { static constexpr auto title = "Slot 2"; };
    struct Slot3 { static constexpr auto title = "Slot 3"; };
    struct Slot4 { static constexpr auto title = "Slot 4"; };
    struct Slot5 { static constexpr auto title = "Slot 5"; };
    struct Slot6 { static constexpr auto title = "Slot 6"; };

    using MaximizedCustom = Entries<
        MaximizedCustomEntry<SaveArmed, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_North>,
        MaximizedCustomEntry<SaveUnarmed, Color::White, UiEvent::ButtonRelease, UiAction::Invoke, SoftButton::Right_North>,
        MaximizedCustomEntry<Slot1, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_West>,
        MaximizedCustomEntry<Slot2, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_Center>,
        MaximizedCustomEntry<Slot3, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_East>,
        MaximizedCustomEntry<Slot4, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthWest>,
        MaximizedCustomEntry<Slot5, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_South>,
        MaximizedCustomEntry<Slot6, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Right_SouthEast>,

        MaximizedCustomEntry<PreviousToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_NorthWest>,
        MaximizedCustomEntry<NextToolbox, Color::White, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_North>,
        MaximizedCustomEntry<GotoToolboxTile, Color::Orange, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_West>,
        MaximizedCustomEntry<GotoToolboxWaveform, Color::LightBlue, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_Center>,
        MaximizedCustomEntry<GotoToolboxSteps, Color::Green, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_East>,
        MaximizedCustomEntry<GotoToolboxMute, Color::Red, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthWest>,
        MaximizedCustomEntry<GotoToolboxReverb, Color::Yellow, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_South>,
        MaximizedCustomEntry<GotoToolboxSnapshots, Color::Magenta, UiEvent::ButtonPress, UiAction::Invoke, SoftButton::Left_SouthEast>
    >;
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
                  MaximizedParameterEntry<Core::ParameterId::MainPlayground4, Color::Magenta,
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
                  MaximizedParameterEntry<Core::ParameterId::Playground4, Color::Magenta, UiEvent::ReleasedKnobRotate,
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
  };*/
}
