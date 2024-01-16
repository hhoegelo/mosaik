#pragma once

#include <tools/SignalSlot.h>
#include <filesystem>
#include <map>
#include <functional>
#include <variant>
#include <optional>

namespace Core
{
  namespace Api
  {
    enum class ParameterId
    {
      GlobalTempo,   // float 20..240 bpm
      GlobalVolume,  // float 0...1

      SampleFile,  // std::filesystem::path
      Pattern,     // std::array<bool, NUM_STEPS>
      Balance,     // float -1...1
      Gain,        // float 0...1
      Mute,        // bool
      Reverse,     // bool
    };

    using Path = std::filesystem::path;
    using Pattern = std::array<bool, NUM_STEPS>;
    using Float = float;
    using Bool = bool;
    using ParameterValue = std::variant<Bool, Float, Path, Pattern>;
    using ChannelId = std::optional<uint32_t>;
    using Step = uint8_t;

    using Connection = Tools::Signals::Connection;
    template <typename S> using Signal = Tools::Signals::Signal<S>;

    namespace Detail
    {
      template <typename T> struct SignalingCache
      {
        T cache;
        Signal<void(T)> sig;
      };
    }

    class Interface
    {
     public:
      virtual ~Interface() = default;

      virtual void setParameter(ChannelId channel, ParameterId parameterId, const ParameterValue &value) = 0;

      Tools::Signals::Connection connect(ChannelId channel, ParameterId id,
                                         const std::function<void(const ParameterValue &)> &cb);

     protected:
      void commit(ChannelId channelId, ParameterId parameterId, const ParameterValue &v);

     private:
      std::map<std::tuple<ChannelId, ParameterId>, Detail::SignalingCache<ParameterValue>> m_parameterCache;
    };
  }

}