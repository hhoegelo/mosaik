#include <cmath>
#include "Interface.h"
#include "ui/midi-ui/Interface.h"

namespace Core::Api
{
  std::vector<TileId> Interface::getSelectedTiles() const
  {
    std::vector<TileId> ret;

    for(auto c = 0; c < NUM_TILES; c++)
      if(get<bool>(getParameter(c, ParameterId::Selected)))
        ret.emplace_back(c);

    return ret;
  }

  Core::Pattern Interface::getMergedPattern() const
  {
    Core::Pattern merged {};

    for(const auto &tile : getSelectedTiles())
    {
      auto pattern = std::get<Core::Pattern>(getParameter(tile, Core::ParameterId::Pattern));

      for(size_t i = 0; i < merged.size(); i++)
        merged[i] |= pattern[i];
    }

    return merged;
  }

  void Interface::setStep(Step step, bool value)
  {
    for(const auto &tileId : getSelectedTiles())
    {
      auto old = std::get<Pattern>(getParameter(tileId, ParameterId::Pattern));
      old[step] = value;
      setParameter(tileId, ParameterId::Pattern, old);
    }
  }

  void Interface::incSelectedTilesParameter(ParameterId parameterId, int steps)
  {
    for(const auto &tileId : getSelectedTiles())
      incParameter(tileId, parameterId, steps);
  }

  void Interface::toggleSelectedTilesParameter(ParameterId parameterId)
  {
    for(const auto &tileId : getSelectedTiles())
      setParameter(tileId, parameterId, !std::get<bool>(getParameter(tileId, parameterId)));
  }

  Step Interface::loopPositionToStep(Dsp::FramePos pos) const
  {
    auto numFramesPerMinute = SAMPLERATE * 60.0;
    auto num16thPerMinute = std::get<float>(getParameter({}, ParameterId::GlobalTempo)) * 4;
    auto framesPer16th = numFramesPerMinute / num16thPerMinute;
    return static_cast<Step>(std::round(static_cast<double>(pos) / framesPer16th));
  }

  ParameterValue Interface::getFirstSelectedTileParameter(ParameterId parameterId) const
  {
    return getParameter(getSelectedTiles().front(), parameterId);
  }
}
