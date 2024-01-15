#include "Core.h"
#include "api/Interface.h"
#include "DataModel.h"
#include <dsp/api/control/Interface.h>
#include <dsp/tools/Tools.h>

namespace Core
{
  namespace Api
  {
    class Mosaik : public Interface
    {
     public:
      using SampleBuffer = std::vector<Dsp::StereoFrame>;

      Mosaik(DataModel &model, Dsp::Api::Control::Interface &dsp)
          : m_model(model)
          , m_dsp(dsp)
      {
        // populate Cache
        setParameter({}, ParameterId::GlobalTempo, 120.0f);
        setParameter({}, ParameterId::GlobalVolume, 1.0f);

        for(int i = 0; i < NUM_CHANNELS; i++)
        {
          setParameter(i, ParameterId::SampleFile, std::filesystem::path {});
          setParameter(i, ParameterId::Pattern, Pattern {});
          setParameter(i, ParameterId::Balance, 0.0f);
          setParameter(i, ParameterId::Gain, 1.0f);
          setParameter(i, ParameterId::Mute, false);
          setParameter(i, ParameterId::Reverse, false);
        }
      }

      ~Mosaik() override = default;

      void setParameter(ChannelId channelId, ParameterId parameterId, const ParameterValue &v) override
      {
        if(!channelId)
          setGlobalParameter(parameterId, v);
        else
          setChannelParameter(m_model.channels[channelId.value()], parameterId, v);

        m_dsp.takeAudioKernel(newDspKernel(m_model));
        commit(channelId, parameterId, v);
      }

      Dsp::Api::Control::AudioKernel *newDspKernel(const DataModel &data) const
      {
        auto r = std::make_unique<Dsp::Api::Control::AudioKernel>();
        r->volume = data.volume;

        auto numFramesPerMinute = SAMPLERATE * 60;
        auto num16thPerMinute = data.tempo * 4;

        r->framesPer16th = numFramesPerMinute / num16thPerMinute;

        for(auto c = 0; c < NUM_CHANNELS; c++)
        {
          const auto &src = data.channels[c];
          auto &tgt = r->channels[c];

          tgt.pattern = src.pattern;
          tgt.audio = loadSample(src.sample);
          auto unbalancedGain = src.gain * src.muted ? 0.f : 1.f;
          tgt.gainLeft
              = data.channels[c].balance < 0 ? unbalancedGain : unbalancedGain * (1.0f - data.channels[c].balance);
          tgt.gainRight
              = data.channels[c].balance > 0 ? unbalancedGain : unbalancedGain * (1.0f + data.channels[c].balance);
          tgt.playbackFrameIncrement = src.playbackDirection == Direction::Forward ? 1 : -1;
        }

        return r.release();
      }

      void setGlobalParameter(const ParameterId &parameterId, const ParameterValue &v)
      {
        switch(parameterId)
        {
          case ParameterId::GlobalVolume:
            m_model.volume = std::clamp(std::get<Float>(v), 0.f, 1.f);
            break;

          case ParameterId::GlobalTempo:
            m_model.tempo = std::clamp(std::get<Float>(v), 20.f, 480.f);
            break;
        }
      }

      void setChannelParameter(DataModel::Channel &channel, const ParameterId &parameterId, const ParameterValue &v)
      {
        switch(parameterId)
        {
          case ParameterId::SampleFile:
            channel.sample = std::get<Path>(v);
            break;

          case ParameterId::Pattern:
            channel.pattern = std::get<Pattern>(v);
            break;

          case ParameterId::Balance:
            channel.balance = std::get<Float>(v);
            break;

          case ParameterId::Gain:
            channel.gain = std::get<Float>(v);
            break;

          case ParameterId::Mute:
            channel.muted = std::get<Bool>(v);
            break;

          case ParameterId::Reverse:
            channel.playbackDirection = std::get<Bool>(v) ? Direction::Backward : Direction::Forward;
            break;
        }
      }

      std::shared_ptr<SampleBuffer> loadSample(const std::filesystem::path &path) const
      {
        auto it = m_sampleFileCache.find(path);

        if(it != m_sampleFileCache.end())
          return it->second;

        auto ret = std::make_shared<SampleBuffer>(Dsp::Tools::loadFile(path));
        m_sampleFileCache[path] = ret;
        return ret;
      }

     private:
      DataModel &m_model;
      Dsp::Api::Control::Interface &m_dsp;

      mutable std::map<std::filesystem::path, std::shared_ptr<SampleBuffer>> m_sampleFileCache;
    };
  }

  Core::Core(Dsp::Api::Control::Interface &dsp)
      : m_dsp(dsp)
      , m_dataModel(std::make_unique<DataModel>())
      , m_api(std::make_unique<Api::Mosaik>(*m_dataModel.get(), m_dsp))
  {
  }

  Core::~Core() = default;

  Api::Interface &Core::getApi() const
  {
    return *m_api;
  }

}