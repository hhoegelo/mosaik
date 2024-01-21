#pragma once

#include <core/Types.h>
#include <sigc++/sigc++.h>

namespace Core::Api
{
  namespace Detail
  {
    template <typename T> struct SignalingCache
    {
      T cache;
      sigc::signal<void(T)> sig;
    };
  }

  class Interface
  {
   public:
    virtual ~Interface() = default;

    virtual void setParameter(TileId tileId, ParameterId parameterId, const ParameterValue &value) = 0;
    ParameterValue getParameter(TileId tileId, ParameterId parameterId) const;

    sigc::connection connect(TileId tileId, ParameterId id, const std::function<void(const ParameterValue &)> &cb);

   protected:
    void commit(TileId tileId, ParameterId parameterId, const ParameterValue &v);

   private:
    std::map<std::tuple<TileId, ParameterId>, Detail::SignalingCache<ParameterValue>> m_parameterCache;
  };
}
