#include "Interface.h"

namespace Core::Api
{
  sigc::connection Interface::connect(TileId tileId, ParameterId parameterId,
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

  ParameterValue Interface::getParameter(TileId tileId, ParameterId parameterId) const
  {
    auto c = m_parameterCache.find(std::make_tuple(tileId, parameterId));
    return c->second.cache;
  }

  std::vector<TileId> Interface::getSelectedTiles() const
  {
    std::vector<TileId> ret;

    for(auto c = 0; c < NUM_TILES; c++)
      if(get<bool>(getParameter(c, ParameterId::Selected)))
        ret.push_back(c);

    return ret;
  }
}
