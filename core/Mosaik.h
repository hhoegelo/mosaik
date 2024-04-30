#pragma once

#include <core/api/Interface.h>
#include <core/DataModel.h>
#include <dsp/AudioKernel.h>
#include <tools/ReactiveVar.h>
#include <dsp/api/control/Interface.h>
#include <dsp/tools/AudioFileDecoder.h>

#include <map>
#include <utility>

namespace Core::Api
{
  class Mosaik : public Interface
  {
    Tools::DeferredComputations m_kernelUpdate;

   public:
    Mosaik(Glib::RefPtr<Glib::MainContext> ctx, DataModel &model, Dsp::Api::Control::Interface &dsp);
    ~Mosaik() override = default;

    void setParameter(TileId tileId, ParameterId parameterId, const ParameterValue &v) override;
    void loadParameter(TileId tileId, ParameterId parameterId, const ParameterValue &value) override;
    void incParameter(TileId tileId, ParameterId parameterId, int steps) override;
    void setPrelistenSample(const Path &path) override;
    [[nodiscard]] ParameterValue getParameter(TileId tileId, ParameterId parameterId) const override;
    [[nodiscard]] Dsp::SharedSampleBuffer getSamples(TileId tileId) const override;

    struct ParamAccess
    {
      std::function<void(const ParameterValue &)> set;
      std::function<void(const ParameterValue &)> load;
      std::function<ParameterValue()> get;
      std::function<void(int steps)> inc;
    };

   private:
    template <typename Parameters, typename... Args> void bindParameters(TileId tileId, Args &...target);
    template <typename Parameters, typename Targets, size_t... idx>
    void bindParameters(std::integer_sequence<size_t, idx...> int_seq, TileId tileId, Targets targets);
    template <typename Parameters, typename Targets, size_t idx> void bindParameter(TileId tileId, Targets targets);
    template <ParameterId id, typename T = ParameterDescription<id>::Type>
    void bindParameter(TileId tileId, Tools::ReactiveVar<T> &target);

    [[nodiscard]] Dsp::AudioKernel *newDspKernel(const DataModel &dataModel) const;

    void translateGlobals(Dsp::AudioKernel *target, const DataModel &source) const;
    void translateTile(const DataModel &dataModel, Dsp::AudioKernel::Tile &tgt, const DataModel::Tile &src) const;

    DataModel &m_model;

    std::map<std::tuple<TileId, ParameterId>, ParamAccess> m_access;
    Dsp::Api::Control::Interface &m_dsp;
    std::vector<Path> getAllSamples(DataModel &model) const;
  };
}
