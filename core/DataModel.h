#pragma once

#include <tools/ReactiveVar.h>
#include <core/ParameterDescriptor.h>
#include <core/Types.h>
#include <cstdint>
#include <filesystem>
#include <array>
#include <set>
#include <chrono>
#include <vector>
#include <concepts>

namespace Core
{
  template <ParameterId ID> using Reactive = Tools::ReactiveVar<typename ParameterDescriptor<ID>::Type>;

  template <ParameterId ID>
  concept HasDefault = requires() { ParameterDescriptor<ID>::defaultValue; };

  template <ParameterId ID> typename ParameterDescriptor<ID>::Type getDefaultValue()
  {
    return {};
  }

  template <ParameterId ID>
    requires HasDefault<ID>
  typename ParameterDescriptor<ID>::Type getDefaultValue()
  {
    return ParameterDescriptor<ID>::defaultValue;
  }

  struct DataModel
  {
    DataModel()
    {
      tiles[0].selected = true;
    }

    struct Tile
    {
      Reactive<ParameterId::SampleFile> sample { getDefaultValue<ParameterId::SampleFile>() };
      Reactive<ParameterId::Pattern> pattern { getDefaultValue<ParameterId::Pattern>() };
      Reactive<ParameterId::Gain> gain { getDefaultValue<ParameterId::Gain>() };
      Reactive<ParameterId::Balance> balance { getDefaultValue<ParameterId::Balance>() };
      Reactive<ParameterId::Shuffle> shuffle { getDefaultValue<ParameterId::Shuffle>() };
      Reactive<ParameterId::Mute> muted { getDefaultValue<ParameterId::Mute>() };
      Reactive<ParameterId::Reverse> reverse { getDefaultValue<ParameterId::Reverse>() };
      Reactive<ParameterId::Selected> selected { getDefaultValue<ParameterId::Selected>() };

      Reactive<ParameterId::EnvelopeFadeInPos> envelopeFadeInPos { getDefaultValue<ParameterId::EnvelopeFadeInPos>() };
      Reactive<ParameterId::EnvelopeFadedInPos> envelopeFadedInPos {
        getDefaultValue<ParameterId::EnvelopeFadedInPos>()
      };
      Reactive<ParameterId::EnvelopeFadeOutPos> envelopeFadeOutPos {
        getDefaultValue<ParameterId::EnvelopeFadeOutPos>()
      };
      Reactive<ParameterId::EnvelopeFadedOutPos> envelopeFadedOutPos {
        getDefaultValue<ParameterId::EnvelopeFadedOutPos>()
      };
      Reactive<ParameterId::TriggerFrame> triggerFrame { getDefaultValue<ParameterId::TriggerFrame>() };

      Reactive<ParameterId::Speed> speed { getDefaultValue<ParameterId::Speed>() };
      Reactive<ParameterId::ReverbSend> reverbSend { getDefaultValue<ParameterId::ReverbSend>() };

      Reactive<ParameterId::Playground1> playground1 { getDefaultValue<ParameterId::Playground1>() };
      Reactive<ParameterId::Playground2> playground2 { getDefaultValue<ParameterId::Playground2>() };
      Reactive<ParameterId::Playground3> playground3 { getDefaultValue<ParameterId::Playground3>() };
      Reactive<ParameterId::Playground4> playground4 { getDefaultValue<ParameterId::Playground4>() };
      Reactive<ParameterId::Playground5> playground5 { getDefaultValue<ParameterId::Playground5>() };
      Reactive<ParameterId::Playground6> playground6 { getDefaultValue<ParameterId::Playground6>() };
      Reactive<ParameterId::Playground7> playground7 { getDefaultValue<ParameterId::Playground7>() };
    };

    std::array<Tile, NUM_TILES> tiles;

    struct Globals
    {
      Reactive<ParameterId::GlobalTempo> tempo { getDefaultValue<ParameterId::GlobalTempo>() };
      Reactive<ParameterId::GlobalTempoMultiplier> tempoMultiplier {
        getDefaultValue<ParameterId::GlobalTempoMultiplier>()
      };
      Reactive<ParameterId::GlobalVolume> volume { getDefaultValue<ParameterId::GlobalVolume>() };
      Reactive<ParameterId::GlobalPrelistenVolume> prelistenVolume {
        getDefaultValue<ParameterId::GlobalPrelistenVolume>()
      };
      Reactive<ParameterId::GlobalReverbRoomSize> reverbRoomSize {
        getDefaultValue<ParameterId::GlobalReverbRoomSize>()
      };
      Reactive<ParameterId::GlobalReverbColor> reverbColor { getDefaultValue<ParameterId::GlobalReverbColor>() };
      Reactive<ParameterId::GlobalReverbPreDelay> reverbPreDelay {
        getDefaultValue<ParameterId::GlobalReverbPreDelay>()
      };
      Reactive<ParameterId::GlobalReverbChorus> reverbChorus { getDefaultValue<ParameterId::GlobalReverbChorus>() };
      Reactive<ParameterId::GlobalReverbReturn> reverbReturn { getDefaultValue<ParameterId::GlobalReverbReturn>() };
      Reactive<ParameterId::GlobalReverbOnOff> reverbOnOff { getDefaultValue<ParameterId::GlobalReverbOnOff>() };
      Reactive<ParameterId::MainPlayground1> playground1 { getDefaultValue<ParameterId::MainPlayground1>() };
      Reactive<ParameterId::MainPlayground2> playground2 { getDefaultValue<ParameterId::MainPlayground2>() };
      Reactive<ParameterId::MainPlayground3> playground3 { getDefaultValue<ParameterId::MainPlayground3>() };
      Reactive<ParameterId::MainPlayground4> playground4 { getDefaultValue<ParameterId::MainPlayground4>() };
      Reactive<ParameterId::MainPlayground5> playground5 { getDefaultValue<ParameterId::MainPlayground5>() };
      Reactive<ParameterId::MainPlayground6> playground6 { getDefaultValue<ParameterId::MainPlayground6>() };
      Reactive<ParameterId::MainPlayground7> playground7 { getDefaultValue<ParameterId::MainPlayground7>() };
    };

    Globals globals;

    Tools::ReactiveVar<Path> prelistenSample { "" };
    Tools::ReactiveVar<uint8_t> prelistenInteractionCounter { 0 };
    Tools::ReactiveVar<std::optional<std::chrono::system_clock::time_point>> tappedOne;
  };
}
