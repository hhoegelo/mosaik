#pragma once

#include "Types.h"

namespace Core
{
  template <ParameterId id> struct ParameterDescriptor;

  template <> struct ParameterDescriptor<ParameterId::GlobalTempo>
  {
    constexpr static ParameterId id = ParameterId::GlobalTempo;
    constexpr static auto name = "tempo";
    using Type = Float;
    constexpr static Type min = 20.0f;
    constexpr static Type max = 400.0f;
    constexpr static Type coarse = 1.0f;
    constexpr static Type defaultValue = 120;
  };

  template <> struct ParameterDescriptor<ParameterId::GlobalVolume>
  {
    constexpr static ParameterId id = ParameterId::GlobalVolume;
    constexpr static auto name = "volume";
    using Type = Float;
    constexpr static Type min = -72.0f;
    constexpr static Type max = 0.0f;
    constexpr static Type coarse = 0.5f;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::GlobalPrelistenVolume>
  {
    constexpr static ParameterId id = ParameterId::GlobalPrelistenVolume;
    constexpr static auto name = "prelisten-volume";
    using Type = Float;
    constexpr static Type min = -72.0f;
    constexpr static Type max = 0.0f;
    constexpr static Type coarse = 0.5f;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::GlobalReverbRoomSize>
  {
    constexpr static ParameterId id = ParameterId::GlobalReverbRoomSize;
    constexpr static auto name = "reverb-room-size";
    using Type = Float;
    constexpr static Type min = 0.0f;
    constexpr static Type max = 1.0f;
    constexpr static Type coarse = 0.01f;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::GlobalReverbColor>
  {
    constexpr static ParameterId id = ParameterId::GlobalReverbColor;
    constexpr static auto name = "reverb-color";
    using Type = Float;
    constexpr static Type min = 0.0f;
    constexpr static Type max = 1.0f;
    constexpr static Type coarse = 0.01f;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::GlobalReverbPreDelay>
  {
    constexpr static ParameterId id = ParameterId::GlobalReverbPreDelay;
    constexpr static auto name = "reverb-pre-delay";
    using Type = Float;
    constexpr static Type min = 0.0f;
    constexpr static Type max = 1.0f;
    constexpr static Type coarse = 0.01f;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::GlobalReverbChorus>
  {
    constexpr static ParameterId id = ParameterId::GlobalReverbChorus;
    constexpr static auto name = "reverb-chorus";
    using Type = Float;
    constexpr static Type min = 0.0f;
    constexpr static Type max = 1.0f;
    constexpr static Type coarse = 0.01f;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::GlobalReverbReturn>
  {
    constexpr static ParameterId id = ParameterId::GlobalReverbReturn;
    constexpr static auto name = "reverb-return";
    using Type = Float;
    constexpr static Type min = -72.0f;
    constexpr static Type max = 0.0f;
    constexpr static Type coarse = 0.5f;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::GlobalReverbOnOff>
  {
    constexpr static ParameterId id = ParameterId::GlobalReverbOnOff;
    constexpr static auto name = "reverb-on-off";
    using Type = OnOff;
    constexpr static Type defaultValue = OnOffValues::On;
  };

  template <> struct ParameterDescriptor<ParameterId::ChannelVolume>
  {
    constexpr static ParameterId id = ParameterId::ChannelVolume;
    constexpr static auto name = "volume";
    using Type = Float;
    constexpr static Type min = -72.0f;
    constexpr static Type max = 0.0f;
    constexpr static Type coarse = 0.5f;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::ChannelReverbPrePost>
  {
    constexpr static ParameterId id = ParameterId::ChannelReverbPrePost;
    constexpr static auto name = "reverb-pre-post";
    using Type = PrePost;
    constexpr static Type defaultValue = PrePostValues::Post;
  };

  template <> struct ParameterDescriptor<ParameterId::ChannelReverbSend>
  {
    constexpr static ParameterId id = ParameterId::ChannelReverbSend;
    constexpr static auto name = "reverb-send";
    using Type = Float;
    constexpr static Type min = -72.0f;
    constexpr static Type max = 0.0f;
    constexpr static Type coarse = 0.5f;
    constexpr static Type defaultValue = -72.f;
  };

  template <> struct ParameterDescriptor<ParameterId::ChannelDelayPrePost>
  {
    constexpr static ParameterId id = ParameterId::ChannelDelayPrePost;
    constexpr static auto name = "delay-pre-post";
    using Type = PrePost;
    constexpr static Type defaultValue = PrePostValues::Post;
  };

  template <> struct ParameterDescriptor<ParameterId::ChannelDelaySend>
  {
    constexpr static ParameterId id = ParameterId::ChannelDelaySend;
    constexpr static auto name = "delay-send";
    using Type = Float;
    constexpr static Type min = -72.0f;
    constexpr static Type max = 0.0f;
    constexpr static Type coarse = 0.5f;
    constexpr static Type defaultValue = -72.f;
  };

  template <> struct ParameterDescriptor<ParameterId::ChannelOnOff>
  {
    constexpr static ParameterId id = ParameterId::ChannelOnOff;
    constexpr static auto name = "on-off";
    using Type = OnOff;
    constexpr static Type defaultValue = OnOffValues::On;
  };

  template <> struct ParameterDescriptor<ParameterId::Selected>
  {
    constexpr static ParameterId id = ParameterId::Selected;
    using Type = Bool;
    constexpr static auto name = "selected";
  };

  template <> struct ParameterDescriptor<ParameterId::Reverse>
  {
    constexpr static ParameterId id = ParameterId::Reverse;
    using Type = Bool;
    constexpr static auto name = "reverse";
  };

  template <> struct ParameterDescriptor<ParameterId::SampleFile>
  {
    constexpr static ParameterId id = ParameterId::SampleFile;
    using Type = Path;
    constexpr static auto name = "sample";
  };

  template <> struct ParameterDescriptor<ParameterId::Pattern>
  {
    constexpr static ParameterId id = ParameterId::Pattern;
    using Type = Pattern;
    constexpr static auto name = "pattern";
  };

  template <> struct ParameterDescriptor<ParameterId::Mute>
  {
    constexpr static ParameterId id = ParameterId::Mute;
    using Type = Bool;
    constexpr static auto name = "mute";
  };

  template <> struct ParameterDescriptor<ParameterId::Balance>
  {
    constexpr static ParameterId id = ParameterId::Balance;
    constexpr static auto name = "balance";
    using Type = Float;
    constexpr static Type min = -1.0f;
    constexpr static Type max = 1.0f;
    constexpr static Type coarse = 0.01f;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::Gain>
  {
    constexpr static ParameterId id = ParameterId::Gain;
    constexpr static auto name = "gain";
    using Type = Float;
    constexpr static Type min = -48.0f;
    constexpr static Type max = 6.0f;
    constexpr static Type coarse = 0.1f;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::Speed>
  {
    constexpr static ParameterId id = ParameterId::Speed;
    constexpr static auto name = "speed";
    using Type = Float;
    constexpr static Type min = -6.0f;
    constexpr static Type max = 6.0f;
    constexpr static Type coarse = 0.01f;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::EnvelopeFadeInPos>
  {
    constexpr static ParameterId id = ParameterId::EnvelopeFadeInPos;
    constexpr static auto name = "envelopeFadeInPos";
    using Type = FramePos;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::EnvelopeFadedInPos>
  {
    constexpr static ParameterId id = ParameterId::EnvelopeFadedInPos;
    constexpr static auto name = "envelopeFadedInPos";
    using Type = FramePos;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::EnvelopeFadeOutPos>
  {
    constexpr static ParameterId id = ParameterId::EnvelopeFadeOutPos;
    constexpr static auto name = "envelopeFadeOutPos";
    using Type = FramePos;
    constexpr static Type defaultValue = std::numeric_limits<FramePos>::max();
  };

  template <> struct ParameterDescriptor<ParameterId::EnvelopeFadedOutPos>
  {
    constexpr static ParameterId id = ParameterId::EnvelopeFadedOutPos;
    constexpr static auto name = "envelopeFadedOutPos";
    using Type = FramePos;
    constexpr static Type defaultValue = std::numeric_limits<FramePos>::max();
  };

  template <> struct ParameterDescriptor<ParameterId::TriggerFrame>
  {
    constexpr static ParameterId id = ParameterId::TriggerFrame;
    constexpr static auto name = "triggerFrame";
    using Type = FramePos;
    constexpr static Type defaultValue = 0;
  };

  template <> struct ParameterDescriptor<ParameterId::Shuffle>
  {
    constexpr static ParameterId id = ParameterId::Shuffle;
    constexpr static auto name = "shuffle";
    using Type = Float;
    constexpr static Type min = -1.0f;
    constexpr static Type max = 1.0f;
    constexpr static Type coarse = 0.01f;
    constexpr static Type defaultValue = 0;
  };

  template <ParameterId P> struct ParameterDescriptionPlayground
  {
    constexpr static ParameterId id = P;
    using Type = float;
    constexpr static Type min = 0.0f;
    constexpr static Type max = 1.0f;
    constexpr static Type coarse = 0.01f;
    constexpr static Type defaultValue = 0;
  };

  template <>
  struct ParameterDescriptor<ParameterId::Playground1> : ParameterDescriptionPlayground<ParameterId::Playground1>
  {
    constexpr static auto name = "playground-1";
  };

  template <>
  struct ParameterDescriptor<ParameterId::Playground2> : ParameterDescriptionPlayground<ParameterId::Playground2>
  {
    constexpr static auto name = "playground-2";
  };

  template <>
  struct ParameterDescriptor<ParameterId::Playground3> : ParameterDescriptionPlayground<ParameterId::Playground3>
  {
    constexpr static auto name = "playground-3";
  };

  template <>
  struct ParameterDescriptor<ParameterId::Playground4> : ParameterDescriptionPlayground<ParameterId::Playground4>
  {
    constexpr static auto name = "playground-4";
  };

  template <>
  struct ParameterDescriptor<ParameterId::Playground5> : ParameterDescriptionPlayground<ParameterId::Playground5>
  {
    constexpr static auto name = "playground-5";
  };

  template <>
  struct ParameterDescriptor<ParameterId::Playground6> : ParameterDescriptionPlayground<ParameterId::Playground6>
  {
    constexpr static auto name = "playground-6";
  };

  template <>
  struct ParameterDescriptor<ParameterId::Playground7> : ParameterDescriptionPlayground<ParameterId::Playground7>
  {
    constexpr static auto name = "playground-7";
  };

  template <>
  struct ParameterDescriptor<ParameterId::MainPlayground1>
      : ParameterDescriptionPlayground<ParameterId::MainPlayground1>
  {
    constexpr static auto name = "main-playground-1";
  };

  template <>
  struct ParameterDescriptor<ParameterId::MainPlayground2>
      : ParameterDescriptionPlayground<ParameterId::MainPlayground2>
  {
    constexpr static auto name = "main-playground-2";
  };

  template <>
  struct ParameterDescriptor<ParameterId::MainPlayground3>
      : ParameterDescriptionPlayground<ParameterId::MainPlayground3>
  {
    constexpr static auto name = "main-playground-3";
  };

  template <>
  struct ParameterDescriptor<ParameterId::MainPlayground4>
      : ParameterDescriptionPlayground<ParameterId::MainPlayground4>
  {
    constexpr static auto name = "main-playground-4";
  };

  template <>
  struct ParameterDescriptor<ParameterId::MainPlayground5>
      : ParameterDescriptionPlayground<ParameterId::MainPlayground5>
  {
    constexpr static auto name = "main-playground-5";
  };

  template <>
  struct ParameterDescriptor<ParameterId::MainPlayground6>
      : ParameterDescriptionPlayground<ParameterId::MainPlayground6>
  {
    constexpr static auto name = "main-playground-6";
  };

  template <>
  struct ParameterDescriptor<ParameterId::MainPlayground7>
      : ParameterDescriptionPlayground<ParameterId::MainPlayground7>
  {
    constexpr static auto name = "main-playground-7";
  };

  template <template <ParameterId> typename Wrapper, ParameterId... ids> struct Parameters
  {
    using Wrapped = std::tuple<typename Wrapper<ids>::Wrapped...>;

    static constexpr bool contains(ParameterId id)
    {
      return ((ids == id) || ...);
    }

    template <typename T> static void forEach(T cb)
    {
      (cb(ParameterDescriptor<ids> {}), ...);
    }
  };

  template <template <ParameterId> typename Wrapper>
  using GlobalParameters
      = Parameters<Wrapper, ParameterId::GlobalTempo, ParameterId::GlobalVolume, ParameterId::GlobalPrelistenVolume,
                   ParameterId::GlobalReverbRoomSize, ParameterId::GlobalReverbColor, ParameterId::GlobalReverbPreDelay,
                   ParameterId::GlobalReverbChorus, ParameterId::GlobalReverbReturn, ParameterId::GlobalReverbOnOff,
                   ParameterId::MainPlayground1, ParameterId::MainPlayground2, ParameterId::MainPlayground3,
                   ParameterId::MainPlayground4, ParameterId::MainPlayground5, ParameterId::MainPlayground6,
                   ParameterId::MainPlayground7>;

  template <template <ParameterId> typename Wrapper>
  using ChannelParameters
      = Parameters<Wrapper, ParameterId::ChannelOnOff, ParameterId::ChannelVolume, ParameterId::ChannelDelayPrePost,
                   ParameterId::ChannelDelaySend, ParameterId::ChannelReverbPrePost, ParameterId::ChannelReverbSend>;

  template <template <ParameterId> typename Wrapper>
  using TileParameters = Parameters<Wrapper, ParameterId::Selected, ParameterId::SampleFile, ParameterId::Reverse,
                                    ParameterId::Pattern, ParameterId::Balance, ParameterId::Gain, ParameterId::Mute,
                                    ParameterId::Speed, ParameterId::EnvelopeFadeInPos, ParameterId::EnvelopeFadedInPos,
                                    ParameterId::EnvelopeFadeOutPos, ParameterId::EnvelopeFadedOutPos,
                                    ParameterId::TriggerFrame, ParameterId::Shuffle, ParameterId::Playground1,
                                    ParameterId::Playground2, ParameterId::Playground3, ParameterId::Playground4,
                                    ParameterId::Playground5, ParameterId::Playground6, ParameterId::Playground7>;

  template <ParameterId id> struct WrapParameterDescriptor
  {
    using Wrapped = ParameterDescriptor<id>;
  };

  template <ParameterId id> struct NoWrap
  {
    using Wrapped = void;
  };

  using GlobalParameterDescriptors = GlobalParameters<WrapParameterDescriptor>::Wrapped;
  using ChannelParameterDescriptors = ChannelParameters<WrapParameterDescriptor>::Wrapped;
  using TileParameterDescriptors = TileParameters<WrapParameterDescriptor>::Wrapped;
}
