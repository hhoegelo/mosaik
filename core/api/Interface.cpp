#include <cmath>
#include "Interface.h"
#include "ui/midi-ui/Interface.h"

namespace Core::Api
{
  Tools::Signals::Connection Interface::connect(TileId tileId, ParameterId parameterId,
                                                const std::function<void(const ParameterValue &)> &cb)
  {
    return m_parameterCache[std::make_tuple(tileId, parameterId)].connectWithInit(cb);
  }

  void Interface::commit(TileId tileId, ParameterId parameterId, const ParameterValue &v)
  {
    m_parameterCache[std::make_tuple(tileId, parameterId)].emit(v);
  }

  ParameterValue Interface::getParameter(Computation *computation, TileId tileId, ParameterId parameterId) const
  {
    auto c = m_parameterCache.find(std::make_tuple(tileId, parameterId));
    if(computation)
      computation->add(&c->second);

    return std::get<0>(c->second.getCache());
  }

  std::vector<TileId> Interface::getSelectedTiles(Computation *computation) const
  {
    std::vector<TileId> ret;

    for(auto c = 0; c < NUM_TILES; c++)
      if(get<bool>(getParameter(computation, c, ParameterId::Selected)))
        ret.emplace_back(c);

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

  ParameterValue Interface::getFirstSelectedTileParameter(Computation *computation, ParameterId id) const
  {
    return getParameter(computation, *getSelectedTiles(computation).begin(), id);
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

  void Interface::incSelectedTilesParameter(ParameterId parameterId, int steps)
  {
    for(const auto &tileId : getSelectedTiles(nullptr))
      incParameter(tileId, parameterId, steps);
  }

  void Interface::toggleSelectedTilesParameter(ParameterId parameterId)
  {
    for(const auto &tileId : getSelectedTiles(nullptr))
      setParameter(tileId, parameterId, !std::get<bool>(getParameter(nullptr, tileId, parameterId)));
  }

  Step Interface::loopPositionToStep(Dsp::FramePos pos) const
  {
    auto numFramesPerMinute = SAMPLERATE * 60.0;
    auto num16thPerMinute = std::get<float>(getParameter(nullptr, {}, ParameterId::GlobalTempo)) * 4;
    auto framesPer16th = numFramesPerMinute / num16thPerMinute;
    return static_cast<Step>(std::round(static_cast<double>(pos) / framesPer16th));
  }
}
