#include <cmath>
#include "Interface.h"
#include "ui/midi-ui/Interface.h"
#include <core/ParameterDescriptor.h>
#include <core/DataModel.h>

#define JSON_ASSERT(x)
#include <tools/json.h>
#include <iostream>

namespace Core::Api
{
  void Interface::init()
  {

    auto i = [this](Address a, auto desc)
    {
      using P = decltype(desc);
      loadParameter(a, desc.id, static_cast<typename P::Type>(getDefaultValue<P::id>()));
    };

    GlobalParameters<NoWrap>::forEach([this, &i](auto desc) { i({}, desc); });

    TileParameters<NoWrap>::forEach(
        [this, &i](auto desc)
        {
          for(auto t = 0; t < NUM_TILES; t++)
            i({ t }, desc);
        });
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
    auto num16thPerMinute = std::get<float>(getParameter({}, ParameterId::GlobalTempo)) * 4
        * std::get<float>(getParameter({}, ParameterId::GlobalTempoMultiplier));
    auto framesPer16th = numFramesPerMinute / num16thPerMinute;
    return static_cast<Step>(std::round(static_cast<double>(pos) / framesPer16th));
  }

  Address Interface::getSelectedTile() const
  {
    for(auto t = 0; t < NUM_TILES; t++)
      if(get<bool>(getParameter({ t }, ParameterId::Selected)))
        return { t };

    return { 0 };
  }
}
