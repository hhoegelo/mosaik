#include <cmath>
#include "Interface.h"
#include "ui/midi-ui/Interface.h"
#include <core/ParameterDescriptor.h>

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

        auto loadParameter = [&](auto json, Address address, auto p)
        {
          if(json.contains(p.name))
            this->loadParameter(address, p.id, static_cast<typename decltype(p)::Type>(json[p.name]));
        };

        if(j.contains("globals"))
          std::apply([&](auto... a) { (loadParameter(j["globals"], {}, a), ...); }, GlobalParameterDescriptors {});

        if(j.contains("channels"))
        {
          for(uint8_t c = 0; c < NUM_CHANNELS; c++)
          {
            const auto &channelJson = j["channels"][c];
            std::apply([&](auto... a) { (loadParameter(channelJson, Address { c, {} }, a), ...); },
                       ChannelParameterDescriptors {});

            for(uint8_t t = 0; t < NUM_TILES_PER_CHANNEL; t++)
            {
              const auto &tileJson = channelJson["tiles"][t];
              std::apply([&](auto... a) { (loadParameter(tileJson, Address { c, t }, a), ...); },
                         TileParameterDescriptors {});
            }
          }
        }
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

    auto saveParameter = [&](auto &json, Address id, auto p)
    { json[p.name] = std::get<typename decltype(p)::Type>(getParameter(id, p.id)); };

    std::apply([&](auto... a) { (saveParameter(j["globals"], Address {}, a), ...); }, GlobalParameterDescriptors {});

    for(uint8_t c = 0; c < NUM_CHANNELS; c++)
    {
      auto &channelJson = j["channels"][c];
      std::apply([&](auto... a) { (saveParameter(channelJson, Address { c, {} }, a), ...); },
                 ChannelParameterDescriptors {});

      for(uint8_t t = 0; t < NUM_TILES_PER_CHANNEL; t++)
      {
        auto &tileJson = channelJson["tiles"][t];
        std::apply([&](auto... a) { (saveParameter(tileJson, Address { c, t }, a), ...); },
                   TileParameterDescriptors {});
      }
    }

    std::ofstream(path) << j;
  }

  void Interface::incSelectedTilesParameter(ParameterId parameterId, int steps)
  {
    auto tileId = getSelectedTile();
    incParameter(tileId, parameterId, steps);
  }

  void Interface::toggleSelectedTilesParameter(ParameterId parameterId)
  {
    auto tileId = getSelectedTile();
    setParameter(tileId, parameterId, !std::get<bool>(getParameter(tileId, parameterId)));
  }

  Step Interface::loopPositionToStep(Dsp::FramePos pos) const
  {
    auto numFramesPerMinute = SAMPLERATE * 60.0;
    auto num16thPerMinute = std::get<float>(getParameter({}, ParameterId::GlobalTempo)) * 4;
    auto framesPer16th = numFramesPerMinute / num16thPerMinute;
    return static_cast<Step>(std::round(static_cast<double>(pos) / framesPer16th));
  }

  Address Interface::getSelectedTile() const
  {
    for(auto c = 0; c < NUM_CHANNELS; c++)
      for(auto t = 0; t < NUM_TILES_PER_CHANNEL; t++)
        if(get<bool>(getParameter({ c, t }, ParameterId::Selected)))
          return { c, t };

    return { 0, 0 };
  }

  void Interface::addTap()
  {
  }
}
