#pragma once

#include <core/Types.h>
#include <tools/SignalSlot.h>

namespace Core::Api
{
  namespace Detail
  {
    template <typename T> struct SignalingCache
    {
      T cache;
      Tools::Signals::Signal<void(T)> sig;
    };
  }

  class Interface
  {
   public:
    virtual ~Interface() = default;

    virtual void setParameter(TileId tileId, ParameterId parameterId, const ParameterValue &value) = 0;

    Tools::Signals::Connection connect(TileId tileId, ParameterId id,
                                       const std::function<void(const ParameterValue &)> &cb);

   protected:
    void commit(TileId tileId, ParameterId parameterId, const ParameterValue &v);

   private:
    std::map<std::tuple<TileId, ParameterId>, Detail::SignalingCache<ParameterValue>> m_parameterCache;
  };
}
