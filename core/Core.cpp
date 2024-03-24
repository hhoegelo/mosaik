#include "Core.h"
#include "api/Interface.h"
#include "DataModel.h"
#include "FadeParameterSanitizer.h"
#include <dsp/api/control/Interface.h>
#include <dsp/AudioKernel.h>
#include <cmath>
#include <cassert>
#include <map>

namespace Core
{
  namespace Api
  {
    struct ParamAccess
    {
      std::function<void(const ParameterValue &)> set;
      std::function<ParameterValue()> get;
      std::function<void(int steps)> inc;
    };

    ParamAccess buildAccess(Tools::ReactiveVar<float> &target, float min, float max, float stepFactor)
    {
      return { .set = [&target, min, max](const auto &v) { target = std::clamp(std::get<float>(v), min, max); },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc = [&target, stepFactor, min, max](int steps)
               { target = std::clamp(target.get() + stepFactor * static_cast<float>(steps), min, max); } };
    }

    ParamAccess buildAccess(Tools::ReactiveVar<bool> &target)
    {
      return { .set = [&target](const auto &v) { target = std::get<bool>(v); },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc = [&target](int steps) { target = !target.get(); } };
    }

    ParamAccess buildExclusiveAccess(DataModel &model, Tools::ReactiveVar<bool> &target)
    {
      return { .set =
                   [&target, &model](const auto &v)
               {
                 for(auto &tile : model.tiles)
                 {
                   auto lhs = &tile.selected;
                   auto rhs = &target;
                   tile.selected = lhs == rhs;
                 }

                 assert(std::count_if(model.tiles.begin(), model.tiles.end(), [](auto &t) { return t.selected.get(); })
                        == 1);
               },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc = [&](int) {} };
    }

    template <typename T> ParamAccess buildAccessGetSet(Tools::ReactiveVar<T> &target)
    {
      return { .set = [&target](const auto &v) { target = std::get<T>(v); },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc = [](int) {} };
    }

    template <typename Sanitizer>
    ParamAccess buildAccess(Dsp::Api::Control::Interface &dsp, Tools::ReactiveVar<FramePos> &target,
                            const Tools::ReactiveVar<std::filesystem::path> &sample, Sanitizer sanitizer)
    {
      return { .set =
                   [&target, &dsp, &sample, sanitizer](const auto &v)
               {
                 target = std::get<FramePos>(v);
                 sanitizer(dsp.getSamples(sample)->size());
               },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc =
                   [&target, &dsp, &sample, sanitizer](int steps)
               {
                 target = target.get() + steps;
                 sanitizer(dsp.getSamples(sample)->size());
               } };
    }

    class Mosaik : public Interface
    {
     public:
      Mosaik(DataModel &model, Dsp::Api::Control::Interface &dsp)
          : m_model(model)
          , m_dsp(dsp)
      {
        m_access
            = { { { TileId {}, ParameterId::GlobalVolume },
                  buildAccess(m_model.globals.volume, c_silenceDB, c_maxDB, 1.f) },
                { { TileId {}, ParameterId::GlobalShuffle }, buildAccess(m_model.globals.shuffle, 0.0f, 1.0f, 0.001f) },
                { { TileId {}, ParameterId::GlobalTempo }, buildAccess(m_model.globals.tempo, 20.0f, 240.0f, 0.1f) } };

        for(auto c = 0; c < NUM_TILES; c++)
        {
          TileId id(c);
          auto &src = m_model.tiles[id.value()];
          m_access[{ id, ParameterId::SampleFile }] = buildAccessGetSet(src.sample);
          m_access[{ id, ParameterId::Pattern }] = buildAccessGetSet(src.pattern);
          m_access[{ id, ParameterId::Balance }] = buildAccess(src.balance, -1.0f, 1.0f, 0.02);
          m_access[{ id, ParameterId::Gain }] = buildAccess(src.gain, c_silenceDB, c_maxDB, 1.f);
          m_access[{ id, ParameterId::Mute }] = buildAccess(src.muted);
          m_access[{ id, ParameterId::Reverse }] = buildAccess(src.reverse);
          m_access[{ id, ParameterId::Speed }] = buildAccess(src.speed, -1.0f, 1.0f, 0.02);
          m_access[{ id, ParameterId::Selected }] = buildExclusiveAccess(m_model, src.selected);

          m_access[{ id, ParameterId::EnvelopeFadeInPos }] = buildAccess(
              m_dsp, src.envelopeFadeInPos, src.sample,
              [&src](size_t sampleLength)
              {
                FadeParameterSanitizer::sanitizeFadeInPos(sampleLength, src.envelopeFadeInPos, src.envelopeFadeInLen,
                                                          src.envelopeFadeOutPos, src.envelopeFadeOutLen);
              });

          m_access[{ id, ParameterId::EnvelopeFadeInLen }] = buildAccess(
              m_dsp, src.envelopeFadeInLen, src.sample,
              [&src](size_t sampleLength)
              {
                FadeParameterSanitizer::sanitizeFadeInLen(sampleLength, src.envelopeFadeInPos, src.envelopeFadeInLen,
                                                          src.envelopeFadeOutPos, src.envelopeFadeOutLen);
              });

          m_access[{ id, ParameterId::EnvelopeFadeOutPos }] = buildAccess(
              m_dsp, src.envelopeFadeOutPos, src.sample,
              [&src](size_t sampleLength)
              {
                FadeParameterSanitizer::sanitizeFadeOutPos(sampleLength, src.envelopeFadeInPos, src.envelopeFadeInLen,
                                                           src.envelopeFadeOutPos, src.envelopeFadeOutLen);
              });

          m_access[{ id, ParameterId::EnvelopeFadeOutLen }] = buildAccess(
              m_dsp, src.envelopeFadeOutLen, src.sample,
              [&src](size_t sampleLength)
              {
                FadeParameterSanitizer::sanitizeFadeOutLen(sampleLength, src.envelopeFadeInPos, src.envelopeFadeInLen,
                                                           src.envelopeFadeOutPos, src.envelopeFadeOutLen);
              });
        }

        m_dsp.takeAudioKernel(newDspKernel(m_model));
      }

      ~Mosaik() override = default;

      void setParameter(TileId tileId, ParameterId parameterId, const ParameterValue &v) override
      {
        m_access.find({ tileId, parameterId })->second.set(v);
        m_dsp.takeAudioKernel(newDspKernel(m_model));
      }

      void incParameter(TileId tileId, ParameterId parameterId, int steps) override
      {
        m_access.find({ tileId, parameterId })->second.inc(steps);
        m_dsp.takeAudioKernel(newDspKernel(m_model));
      }

      [[nodiscard]] ParameterValue getParameter(TileId tileId, ParameterId parameterId) const override
      {
        return m_access.find({ tileId, parameterId })->second.get();
      }

      [[nodiscard]] Dsp::SharedSampleBuffer getSamples(TileId tileId) const override
      {
        return m_dsp.getSamples(get<std::filesystem::path>(getParameter(tileId, ParameterId::SampleFile)));
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

            tgt.triggers.push_back(pos + shuffle);
          }

          pos += framesPer16th;
        }

        tgt.audio = m_dsp.getSamples(src.sample);
        tgt.mute = src.muted;
        tgt.balance = src.balance;
        tgt.gain_dB = src.gain;
        tgt.playbackFrameIncrement = powf(2.0f, src.speed * 2);
        tgt.reverse = src.reverse;

        auto calcM
            = [](float startY, float endY, FramePos l) { return l ? (endY - startY) / static_cast<float>(l) : 0.0f; };
        auto calcB = [&](float startY, float endY, FramePos p, FramePos l)
        { return startY - calcM(startY, endY, l) * static_cast<float>(p); };

        // faded-out section
        tgt.envelope[0] = { src.envelopeFadeOutPos + src.envelopeFadeOutLen, c_zeroDB, c_silenceDB };

        // fade-out section
        tgt.envelope[1] = { src.envelopeFadeOutPos, calcM(c_zeroDB, c_silenceDB, src.envelopeFadeOutLen),
                            calcB(c_zeroDB, c_silenceDB, src.envelopeFadeOutPos, src.envelopeFadeOutLen) };

        // faded-in section
        tgt.envelope[2] = { src.envelopeFadeInPos + src.envelopeFadeInLen, 0.0f, c_zeroDB };

        // fade-in section
        tgt.envelope[3] = { src.envelopeFadeInPos, calcM(c_silenceDB, c_zeroDB, src.envelopeFadeInLen),
                            calcB(c_silenceDB, c_zeroDB, src.envelopeFadeInPos, src.envelopeFadeInLen) };

        // pre fade-in section
        tgt.envelope[4] = { 0, 0, c_silenceDB };
      }

     private:
      DataModel &m_model;
      std::map<std::tuple<TileId, ParameterId>, ParamAccess> m_access;
      Dsp::Api::Control::Interface &m_dsp;
    };
  }

  static std::filesystem::path getInitFileName()
  {
    std::filesystem::path home = getenv("HOME");
    return home / ".mosaik";
  }

  Core::Core(Dsp::Api::Control::Interface &dsp, std::unique_ptr<DataModel> dataModel)
      : m_dsp(dsp)
      , m_dataModel(dataModel ? std::move(dataModel) : std::make_unique<DataModel>(getInitFileName()))
      , m_api(std::make_unique<Api::Mosaik>(*m_dataModel, m_dsp))
  {
  }

  Core::~Core() = default;

  Api::Interface &Core::getApi() const
  {
    return *m_api;
  }
}