#pragma once

#include <core/api/Interface.h>
#include <core/DataModel.h>
#include <dsp/AudioKernel.h>
#include <tools/ReactiveVar.h>
#include <dsp/api/control/Interface.h>
#include <dsp/tools/AudioFileDecoder.h>

#include <map>
#include <utility>

namespace Core
{
  template <ParameterId id> struct ParameterDescriptor;
}

namespace Core::Api
{
  class Mosaik : public Interface
  {
    Tools::DeferredComputations m_kernelUpdate;

   public:
    Mosaik(Glib::RefPtr<Glib::MainContext> ctx, DataModel &model, Dsp::Api::Control::Interface &dsp);
    ~Mosaik() override = default;

    void setParameter(Address address, ParameterId parameterId, const ParameterValue &v) override;
    void loadParameter(Address address, ParameterId parameterId, const ParameterValue &value) override;
    void incParameter(Address address, ParameterId parameterId, int steps) override;
    void setPrelistenSample(const Path &path) override;
    [[nodiscard]] ParameterValue getParameter(Address address, ParameterId parameterId) const override;
    [[nodiscard]] Dsp::SharedSampleBuffer getSamples(Address address) const override;
    void addTap() override;
    void setOne() override;

    struct ParamAccess
    {
      std::function<void(const ParameterValue &)> set;
      std::function<void(const ParameterValue &)> load;
      std::function<ParameterValue()> get;
      std::function<void(int steps)> inc;
    };

   private:
    const Mosaik::ParamAccess &findAccess(Address address, ParameterId parameterId) const;

    template <typename Parameters, typename... Args> void bindParameters(Address address, Args &...target);
    template <typename Parameters, typename Targets, size_t... idx>
    void bindParameters(std::integer_sequence<size_t, idx...> int_seq, Address address, Targets targets);
    template <typename Parameters, typename Targets, size_t idx> void bindParameter(Address address, Targets targets);
    template <ParameterId id, typename T = ParameterDescriptor<id>::Type>
    void bindParameter(Address address, Tools::ReactiveVar<T> &target);

    [[nodiscard]] Dsp::AudioKernel *newDspKernel(const DataModel &dataModel) const;

    void translateGlobals(Dsp::AudioKernel *target, const DataModel &source) const;
    void translateChannel(const DataModel &dataModel, Dsp::AudioKernel::Channel &tgt,
                          const DataModel::Channel &src) const;
    void translateTile(const DataModel &dataModel, Dsp::AudioKernel::Channel::Tile &tgt,
                       const DataModel::Channel::Tile &src) const;

    DataModel &m_model;
    std::vector<std::chrono::system_clock::time_point> m_taps;
    std::map<std::tuple<Address, ParameterId>, ParamAccess> m_access;
    Dsp::Api::Control::Interface &m_dsp;
    std::vector<Path> getAllSamples(DataModel &model) const;

    Tools::DeferredComputations m_sanitizeSamplePositions;
  };
}
