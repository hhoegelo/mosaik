#include "Interface.h"
#include "ui/midi-ui/Interface.h"

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

  ParameterValue Interface::getParameter(Computation *computation, TileId tileId, ParameterId parameterId) const
  {
    auto c = m_parameterCache.find(std::make_tuple(tileId, parameterId));
    if(computation)
      computation->add(&c->second.sig);

    return c->second.cache;
  }

  std::vector<TileId> Interface::getSelectedTiles(Computation *computation) const
  {
    std::vector<TileId> ret;

    for(auto c = 0; c < NUM_TILES; c++)
      if(get<bool>(getParameter(computation, c, ParameterId::Selected)))
        ret.push_back(c);

    return ret;
  }

  Core::Pattern Interface::getMergedPattern(Computation *computation) const
  {
    Core::Pattern merged {};

    for(const auto &tile : getSelectedTiles(computation))
    {
      auto pattern = std::get<Core::Pattern>(getParameter(computation, tile, Core::ParameterId::Pattern));

      for(size_t i = 0; i < merged.size(); i++)
        merged[i] |= pattern[i];
    }

    return merged;
  }

  void Interface::setStep(Step step, bool value)
  {
    for(const auto &tileId : getSelectedTiles(nullptr))
    {
      auto old = std::get<Pattern>(getParameter(nullptr, tileId, ParameterId::Pattern));
      old[step] = value;
      setParameter(tileId, ParameterId::Pattern, old);
    }
  }
}
