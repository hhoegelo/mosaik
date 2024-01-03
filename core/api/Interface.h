#pragma once

#include <tools/SignalSlot.h>
#include <filesystem>
#include <map>
#include <functional>

namespace Core
{
  namespace Api
  {
    enum class ParameterId
    {
      Foo,
      Bar,
      Baz,
      Tempo,
      MainVolume
    };

    using Col = uint8_t;
    using Row = uint8_t;
    using ParameterValue = float;
    using Path = std::filesystem::path;
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

      virtual void setParameter(Col col, Row row, ParameterId id, ParameterValue v) = 0;
      virtual void setParameter(ParameterId id, ParameterValue v) = 0;
      virtual void loadSample(Col col, Row row, const Path &path) = 0;

      [[nodiscard]] Tools::Signals::Connection connect(Col col, Row row, ParameterId id,
                                                       const std::function<void(ParameterValue)> &cb);
      [[nodiscard]] Tools::Signals::Connection connect(Col col, Row row, const std::function<void(const Path &)> &cb);
      [[nodiscard]] Tools::Signals::Connection connect(ParameterId id, const std::function<void(ParameterValue)> &cb);

     protected:
      void commit(Col col, Row row, ParameterId id, ParameterValue v);
      void commit(Col col, Row row, const Path &);
      void commit(ParameterId id, ParameterValue v);

     private:
      std::map<std::tuple<Col, Row, ParameterId>, Detail::SignalingCache<ParameterValue>> m_perChannelParameterCache;
      std::map<std::tuple<Col, Row>, Detail::SignalingCache<Path>> m_perChannelPathCache;
      std::map<ParameterId, Detail::SignalingCache<ParameterValue>> m_globalParameterCache;
    };
  }

}