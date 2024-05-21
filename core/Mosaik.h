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

    void load(const Path &path) override;
    void save(const Path &path) override;

    void saveSnapshot(int id) override;
    void loadSnapshot(int id) override;

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

    template <typename Parameters, typename Tuple> void bindParameters(Address address, Tuple &target);
    template <typename Description, typename Tuple> void bindParameter(Address address, Tuple &target);

    [[nodiscard]] Dsp::AudioKernel *newDspKernel(const DataModel &dataModel) const;

    void translateGlobals(Dsp::AudioKernel *target, const DataModel &source) const;
    void translateTile(const DataModel &dataModel, Dsp::AudioKernel::Tile &tgt, const Address &src) const;

    DataModel &m_model;
    std::vector<std::chrono::system_clock::time_point> m_taps;
    std::map<std::tuple<Address, ParameterId>, ParamAccess> m_access;
    Dsp::Api::Control::Interface &m_dsp;
    std::vector<Path> getAllSamples(DataModel &model) const;

    Tools::DeferredComputations m_sanitizeSamplePositions;
    void sanitizeSamplePositions(const Address &a) const;
    void updateAudioKernel();
  };
}
