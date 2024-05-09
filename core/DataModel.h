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

namespace Core
{
  template <ParameterId ID> using Reactive = Tools::ReactiveVar<typename ParameterDescriptor<ID>::Type>;

  struct DataModel
  {
    DataModel()
    {
      channels[0].tiles[0].selected = true;
    }

    struct Channel
    {
      struct Tile
      {
        Reactive<ParameterId::SampleFile> sample { "" };
        Reactive<ParameterId::Pattern> pattern { {} };
        Reactive<ParameterId::Gain> gain { 0.f };
        Reactive<ParameterId::Balance> balance { 0.f };
        Reactive<ParameterId::Shuffle> shuffle { 0.f };
        Reactive<ParameterId::Mute> muted { false };
        Reactive<ParameterId::Reverse> reverse { false };
        Reactive<ParameterId::Selected> selected { false };

        Reactive<ParameterId::EnvelopeFadeInPos> envelopeFadeInPos { 0 };
        Reactive<ParameterId::EnvelopeFadedInPos> envelopeFadedInPos { 0 };
        Reactive<ParameterId::EnvelopeFadeOutPos> envelopeFadeOutPos { std::numeric_limits<FramePos>::max() };
        Reactive<ParameterId::EnvelopeFadedOutPos> envelopeFadedOutPos { std::numeric_limits<FramePos>::max() };
        Reactive<ParameterId::TriggerFrame> triggerFrame { 0 };

        Reactive<ParameterId::Speed> speed { 0 };

        Reactive<ParameterId::Playground1> playground1 { 0.f };
        Reactive<ParameterId::Playground2> playground2 { 0.f };
        Reactive<ParameterId::Playground3> playground3 { 0.f };
        Reactive<ParameterId::Playground4> playground4 { 0.f };
        Reactive<ParameterId::Playground5> playground5 { 0.f };
        Reactive<ParameterId::Playground6> playground6 { 0.f };
        Reactive<ParameterId::Playground7> playground7 { 0.f };
      };

      Reactive<ParameterId::ChannelVolume> volume { 0 };
      Reactive<ParameterId::ChannelReverbPrePost> reverbPrePost { PrePost::Post };
      Reactive<ParameterId::ChannelReverbSend> reverbSend { 0.f };
      Reactive<ParameterId::ChannelDelayPrePost> delayPrePost { PrePost::Post };
      Reactive<ParameterId::ChannelDelaySend> delaySend { 0.f };
      Reactive<ParameterId::ChannelOnOff> onOff { OnOff::On };

      std::array<Tile, NUM_TILES_PER_CHANNEL> tiles;
    };

    std::array<Channel, NUM_CHANNELS> channels;

    struct Globals
    {
      Reactive<ParameterId::GlobalTempo> tempo { 120.f };
      Reactive<ParameterId::GlobalVolume> volume { 0 };
      Reactive<ParameterId::MainPlayground1> playground1 { 0.f };
      Reactive<ParameterId::MainPlayground2> playground2 { 0.f };
      Reactive<ParameterId::MainPlayground3> playground3 { 0.f };
      Reactive<ParameterId::MainPlayground4> playground4 { 0.f };
      Reactive<ParameterId::MainPlayground5> playground5 { 0.f };
      Reactive<ParameterId::MainPlayground6> playground6 { 0.f };
      Reactive<ParameterId::MainPlayground7> playground7 { 0.f };
    };

    Globals globals;

    Tools::ReactiveVar<Path> prelistenSample { "" };
    Tools::ReactiveVar<uint8_t> prelistenInteractionCounter { 0 };
    Tools::ReactiveVar<std::optional<std::chrono::system_clock::time_point>> tappedOne;
  };
}
