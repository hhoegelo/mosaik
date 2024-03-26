#include <cmath>
#include "Interface.h"
#include "ui/midi-ui/Interface.h"

#define JSON_ASSERT(x)
#include <tools/json.h>
#include <fstream>
#include <iostream>

namespace Core::Api
{
  void Interface::load(const Path &path)
  {
    if(exists(path))
    {
      try
      {
        nlohmann::json j;
        std::ifstream(path) >> j;

        auto loadParameter = [&](auto json, TileId id, auto p)
        {
          if(json.contains(p.name))
            setParameter(id, p.id, static_cast<typename decltype(p)::Type>(json[p.name]));
        };

        if(j.contains("globals"))
          std::apply([&](auto... a) { (loadParameter(j["globals"], {}, a), ...); }, GlobalParameters::Descriptors {});

        if(j.contains("tiles"))
          for(uint8_t i = 0; i < NUM_TILES; i++)
            std::apply([&](auto... a) { (loadParameter(j["tiles"][i], TileId { i }, a), ...); },
                       TileParameters::Descriptors {});
      }
      catch(...)
      {
        std::cerr << "Could not read initial setup file." << std::endl;
      }
    }
  }

  void Interface::save(const Path &path)
  {
    nlohmann::json j;

    auto saveParameter = [&](auto &json, TileId id, auto p)
    { json[p.name] = std::get<typename decltype(p)::Type>(getParameter(id, p.id)); };

    std::apply([&](auto... a) { (saveParameter(j["globals"], {}, a), ...); }, GlobalParameters::Descriptors {});

    for(uint8_t i = 0; i < NUM_TILES; i++)
      std::apply([&](auto... a) { (saveParameter(j["tiles"][i], TileId { i }, a), ...); },
                 TileParameters::Descriptors {});

    std::ofstream(path) << j;
  }

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

  std::string Interface::getFirstSelectedTileParameterDisplay(ParameterId parameterId) const
  {
    return getParameterDisplay(getSelectedTiles().front(), parameterId);
  }

}
