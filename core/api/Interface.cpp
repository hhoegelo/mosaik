#include "Interface.h"

namespace Core::Api
{
  Tools::Signals::Connection Interface::connect(TileId tileId, ParameterId parameterId,
                                                const std::function<void(const ParameterValue &)> &cb)
  {
    auto &c = m_parameterCache[std::make_tuple(tileId, parameterId)];
    cb(c.cache);
    return c.sig.connect(cb);
  }

  void Interface::commit(TileId tileId, ParameterId parameterId, const ParameterValue &v)
  {
    auto &c = m_parameterCache[std::make_tuple(tileId, parameterId)];
    c.cache = v;
    c.sig.emit(v);
  }
}
