#pragma once

#include <core/Types.h>
#include <dsp/Types.h>

namespace Core::Api
{
  using Step = uint8_t;

  class Interface
  {
   public:
    virtual ~Interface() = default;

    virtual void init();

    virtual void load(const Path &path) = 0;
    virtual void save(const Path &path) = 0;

    virtual void setParameter(Address address, ParameterId parameterId, const ParameterValue &value) = 0;
    virtual void loadParameter(Address address, ParameterId parameterId, const ParameterValue &value) = 0;
    virtual void incParameter(Address address, ParameterId parameterId, int steps) = 0;
    virtual void setPrelistenSample(const Path &path) = 0;

    virtual void saveSnapshot(int id) = 0;
    virtual void loadSnapshot(int id) = 0;

    [[nodiscard]] virtual ParameterValue getParameter(Address address, ParameterId parameterId) const = 0;
    [[nodiscard]] virtual Dsp::SharedSampleBuffer getSamples(Address address) const = 0;
    virtual void addTap() = 0;
    virtual void setOne() = 0;

    // Convenience
    [[nodiscard]] Address getSelectedTile() const;
    [[nodiscard]] Step loopPositionToStep(Dsp::FramePos pos) const;
    void incSelectedTilesParameter(ParameterId parameterId, int steps);
    void toggleSelectedTilesParameter(ParameterId parameterId);
  };
}
