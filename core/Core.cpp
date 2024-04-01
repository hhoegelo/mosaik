#include "Core.h"
#include "api/Interface.h"
#include "DataModel.h"
#include <dsp/api/control/Interface.h>
#include <dsp/AudioKernel.h>
#include <cmath>
#include <map>
#include <utility>

namespace Core
{
  namespace Api
  {
    using IDsp = Dsp::Api::Control::Interface;
    using ICore = Interface;

    struct ParamAccess
    {
      std::function<void(const ParameterValue &)> set;
      std::function<void(const ParameterValue &)> load;
      std::function<ParameterValue()> get;
      std::function<void(int steps)> inc;
      std::function<std::string(const ParameterValue &)> display;
    };

    template <ParameterId id, typename V> struct Binder
    {
      static ParamAccess bind(ICore &, IDsp &, TileId, Tools::ReactiveVar<V> &target)
      {
        using T = ParameterDescription<id>;
        return { .set = [&target](const auto &v) { target = std::get<V>(v); },
                 .load = [&target](const auto &v) { target = std::get<V>(v); },
                 .get = [&target]() -> ParameterValue { return target.get(); },
                 .inc = [](int) {},
                 .display = [](const ParameterValue &v) { return T::format(std::get<V>(v)); }

        };
      }
    };

    template <ParameterId id> struct Binder<id, float>
    {
      static ParamAccess bind(ICore &, IDsp &, TileId, Tools::ReactiveVar<float> &target)
      {
        using T = ParameterDescription<id>;
        return { .set = [&target](const auto &v) { target = std::clamp(std::get<float>(v), T::min, T::max); },
                 .load = [&target](const auto &v) { target = std::get<float>(v); },
                 .get = [&target]() -> ParameterValue { return target.get(); },
                 .inc = [&target](int steps)
                 { target = std::clamp(target.get() + T::coarse * static_cast<float>(steps), T::min, T::max); },
                 .display = [](const ParameterValue &v) { return T::format(std::get<float>(v)); } };
      }
    };

    template <ParameterId id> struct Binder<id, bool>
    {
      static ParamAccess bind(ICore &, IDsp &, TileId, Tools::ReactiveVar<bool> &target)
      {
        using T = ParameterDescription<id>;
        return { .set = [&target](const auto &v) { target = std::get<bool>(v); },
                 .load = [&target](const auto &v) { target = std::get<bool>(v); },
                 .get = [&target]() -> ParameterValue { return target.get(); },
                 .inc =
                     [&target](int steps)
                 {
                   if(steps % 1)
                     target = !target.get();
                 },
                 .display = [](const ParameterValue &v) { return T::format(std::get<bool>(v)); } };
      }
    };

    template <> struct Binder<ParameterId::Selected, bool>
    {
      static ParamAccess bind(ICore &core, IDsp &, TileId tileId, Tools::ReactiveVar<bool> &target)
      {
        using T = ParameterDescription<ParameterId::Selected>;
        return { .set =
                     [&target, &core, tileId](const auto &v)
                 {
                   target = std::get<bool>(v);
                   if(target.get())
                   {
                     for(uint8_t t = 0; t < NUM_TILES; t++)
                     {
                       if(t != tileId && std::get<bool>(core.getParameter(t, ParameterId::Selected)))
                       {
                         core.setParameter(t, ParameterId::Selected, false);
                       }
                     }
                   }
                 },
                 .load = [&target](const auto &v) { target = std::get<bool>(v); },
                 .get = [&target]() -> ParameterValue { return target.get(); },
                 .inc = [](int) {},
                 .display = [](const ParameterValue &v) { return T::format(std::get<bool>(v)); } };
      }
    };

    template <ParameterId id> void sanitizeLeft(FramePos desired, ICore &core, IDsp &dsp, TileId tileId)
    {
      using T = ParameterDescription<id>;
      auto v = std::get<FramePos>(core.getParameter(tileId, id));
      if(v > desired)
        core.setParameter(tileId, id, desired);
    }

    template <> void sanitizeLeft<ParameterId::SampleFile>(FramePos desired, ICore &core, IDsp &dsp, TileId tileId)
    {
    }

    template <ParameterId id> void sanitizeRight(FramePos desired, ICore &core, IDsp &dsp, TileId tileId)
    {
      using T = ParameterDescription<id>;
      auto v = std::get<FramePos>(core.getParameter(tileId, id));
      if(v < desired)
        core.setParameter(tileId, id, desired);
    }

    template <> void sanitizeRight<ParameterId::SampleFile>(FramePos desired, ICore &core, IDsp &dsp, TileId tileId)
    {
    }

    template <ParameterId id> struct Binder<id, FramePos>
    {
      static ParamAccess bind(ICore &core, IDsp &dsp, TileId tileId, Tools::ReactiveVar<FramePos> &target)
      {
        using T = ParameterDescription<id>;

        auto set = [&target, &core, &dsp, tileId](const ParameterValue &v)
        {
          auto oldValue = std::get<FramePos>(core.getParameter(tileId, id));
          auto sample = std::get<Path>(core.getParameter(tileId, ParameterId::SampleFile));
          auto numFrames = static_cast<FramePos>(dsp.getSamples(sample)->size());
          target = std::clamp<FramePos>(std::get<FramePos>(v), 0, numFrames);

          if(oldValue > target)
            sanitizeLeft<T::left>(target.get(), core, dsp, tileId);
          else if(oldValue < target)
            sanitizeRight<T::right>(target.get(), core, dsp, tileId);
        };

        return { .set = set,
                 .load = [&target](const auto &v) { target = std::get<FramePos>(v); },
                 .get = [&target]() -> ParameterValue { return target.get(); },
                 .inc = [&target, set](int steps) { set(target.get() + steps); } };
      }
    };

    class Mosaik : public Interface
    {
      Tools::DeferredComputations m_kernelUpdate;

     public:
      Mosaik(Glib::RefPtr<Glib::MainContext> ctx, DataModel &model, Dsp::Api::Control::Interface &dsp)
          : m_model(model)
          , m_dsp(dsp)
          , m_kernelUpdate(std::move(ctx), 0)
      {
        bindParameters<GlobalParameters>(TileId {}, m_model.globals.tempo, m_model.globals.volume,
                                         m_model.globals.shuffle);

        for(auto c = 0; c < NUM_TILES; c++)
        {
          auto &src = m_model.tiles[c];
          bindParameters<TileParameters>(c, src.selected, src.sample, src.reverse, src.pattern, src.balance, src.gain,
                                         src.muted, src.speed, src.envelopeFadeInPos, src.envelopeFadedInPos,
                                         src.envelopeFadeOutPos, src.envelopeFadedOutPos, src.triggerFrame);
        }

        m_kernelUpdate.add([this] { m_dsp.takeAudioKernel(newDspKernel(m_model)); });
      }

      ~Mosaik() override = default;

      void load(const Path &path) override
      {
        Interface::load(path);
      }

      void setParameter(TileId tileId, ParameterId parameterId, const ParameterValue &v) override
      {
        m_access.find({ tileId, parameterId })->second.set(v);
      }

      void incParameter(TileId tileId, ParameterId parameterId, int steps) override
      {
        m_access.find({ tileId, parameterId })->second.inc(steps);
      }

      [[nodiscard]] ParameterValue getParameter(TileId tileId, ParameterId parameterId) const override
      {
        return m_access.find({ tileId, parameterId })->second.get();
      }

      [[nodiscard]] std::string getParameterDisplay(TileId tileId, ParameterId parameterId) const override
      {
        const auto &a = m_access.find({ tileId, parameterId })->second;
        return a.display(a.get());
      }

      [[nodiscard]] Dsp::SharedSampleBuffer getSamples(TileId tileId) const override
      {
        return m_dsp.getSamples(get<std::filesystem::path>(getParameter(tileId, ParameterId::SampleFile)));
      }

     private:
      template <typename Parameters, typename... Args> void bindParameters(TileId tileId, Args &...target)
      {
        using Indizes = std::make_index_sequence<std::tuple_size_v<typename Parameters::Descriptors>>;
        bindParameters<Parameters>(Indizes {}, tileId, std::make_tuple(std::ref(target)...));
      }

      template <typename Parameters, typename Targets, size_t... idx>
      void bindParameters(std::integer_sequence<size_t, idx...> int_seq, TileId tileId, Targets targets)
      {
        (bindParameter<Parameters, Targets, idx>(tileId, targets), ...);
      }

      template <typename Parameters, typename Targets, size_t idx> void bindParameter(TileId tileId, Targets targets)
      {
        using D = typename std::tuple_element_t<idx, typename Parameters::Descriptors>;
        bindParameter<D::id>(tileId, std::get<idx>(targets));
      }

      template <ParameterId id, typename T = ParameterDescription<id>::Type>
      void bindParameter(TileId tileId, Tools::ReactiveVar<T> &target)
      {
        m_access[{ tileId, id }] = Binder<id, T>::bind(*this, m_dsp, tileId, target);
      }

      [[nodiscard]] Dsp::AudioKernel *newDspKernel(const DataModel &dataModel) const
      {
        auto r = std::make_unique<Dsp::AudioKernel>();
        translateGlobals(r, dataModel);

        for(auto c = 0; c < NUM_TILES; c++)
        {
          const auto &src = dataModel.tiles[c];
          auto &tgt = r->tiles[c];
          translateTile(dataModel, tgt, src);
        }

        return r.release();
      }

      void translateGlobals(auto &target, const DataModel &source) const
      {
        auto numFramesPerMinute = SAMPLERATE * 60.0f;
        auto num16thPerMinute = source.globals.tempo * 4;

        target->framesPer16th = numFramesPerMinute / num16thPerMinute;
        target->framesPerLoop = target->framesPer16th * NUM_STEPS;
        target->volume_dB = source.globals.volume;
      }

      void translateTile(const DataModel &dataModel, Dsp::AudioKernel::Tile &tgt, const DataModel::Tile &src) const
      {
        auto numFramesPerMinute = SAMPLERATE * 60.0f;
        auto num16thPerMinute = dataModel.globals.tempo * 4;
        auto framesPer16th = static_cast<Dsp::FramePos>(numFramesPerMinute / num16thPerMinute);
        auto framePerLoop = framesPer16th * 64;

        Dsp::FramePos pos = 0;

        for(auto s : src.pattern.get())
        {
          if(s)
          {
            auto step = pos / framesPer16th;
            int64_t shuffle = 0;

            if(step % 2)
            {
              shuffle
                  = static_cast<int64_t>(0.5 * static_cast<double>(framesPer16th) * (dataModel.globals.shuffle - 0.5));
            }

            auto finalPos = pos + shuffle - src.triggerFrame;

            while(finalPos < 0)
              finalPos += framePerLoop;

            tgt.triggers.push_back(finalPos % framePerLoop);
          }

          std::sort(tgt.triggers.begin(), tgt.triggers.end());

          pos += framesPer16th;
        }

        tgt.audio = m_dsp.getSamples(src.sample);
        tgt.mute = src.muted;
        tgt.balance = src.balance;
        tgt.gain_dB = src.gain;
        tgt.playbackFrameIncrement = powf(2.0f, src.speed / 100.0f);
        tgt.reverse = src.reverse;

        auto calcM
            = [](float startY, float endY, FramePos l) { return l ? (endY - startY) / static_cast<float>(l) : 0.0f; };
        auto calcB = [&](float startY, float endY, FramePos p, FramePos l)
        { return startY - calcM(startY, endY, l) * static_cast<float>(p); };

        constexpr int c_silenceDB = -60;
        constexpr int c_zeroDB = 0.f;

        auto &fadedOutSection = tgt.envelope[0];
        auto &fadeOutSection = tgt.envelope[1];
        auto &fadedInSection = tgt.envelope[2];
        auto &fadeInSection = tgt.envelope[3];
        auto &preFadeInSection = tgt.envelope[4];

        fadedOutSection = { src.envelopeFadedOutPos, c_zeroDB, c_silenceDB };

        const auto fadeOutLen = src.envelopeFadedOutPos - src.envelopeFadeOutPos;
        fadeOutSection = { src.envelopeFadeOutPos, calcM(c_zeroDB, c_silenceDB, fadeOutLen),
                           calcB(c_zeroDB, c_silenceDB, src.envelopeFadeOutPos, fadeOutLen) };

        fadedInSection = { src.envelopeFadedInPos, 0.0f, c_zeroDB };

        const auto fadeInLen = src.envelopeFadedInPos - src.envelopeFadeInPos;
        fadeInSection = { src.envelopeFadeInPos, calcM(c_silenceDB, c_zeroDB, fadeInLen),
                          calcB(c_silenceDB, c_zeroDB, src.envelopeFadeInPos, fadeInLen) };

        preFadeInSection = { 0, 0, c_silenceDB };
      }

     private:
      DataModel &m_model;
      std::map<std::tuple<TileId, ParameterId>, ParamAccess> m_access;
      Dsp::Api::Control::Interface &m_dsp;
    };
  }

  Core::Core(Dsp::Api::Control::Interface &dsp, const Glib::RefPtr<Glib::MainContext> &ctx,
             std::unique_ptr<DataModel> dataModel)
      : m_dsp(dsp)
      , m_dataModel(dataModel ? std::move(dataModel) : std::make_unique<DataModel>())
      , m_api(std::make_unique<Api::Mosaik>(ctx, *m_dataModel, m_dsp))
  {
  }

  Core::~Core() = default;

  Api::Interface &Core::getApi() const
  {
    return *m_api;
  }

}