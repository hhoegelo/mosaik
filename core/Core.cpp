#include "Core.h"
#include "api/Interface.h"
#include "DataModel.h"
#include <dsp/api/control/Interface.h>
#include <dsp/AudioKernel.h>

namespace Core
{
  namespace Api
  {
    class Mosaik : public Interface
    {
     public:
      Mosaik(DataModel &model, Dsp::Api::Control::Interface &dsp)
          : m_model(model)
          , m_dsp(dsp)
      {
        populateCache();
        m_dsp.takeAudioKernel(newDspKernel(m_model));
      }

      ~Mosaik() override = default;

      void populateCache()
      {
        commit({}, ParameterId::GlobalTempo, m_model.tempo);
        commit({}, ParameterId::GlobalVolume, m_model.volume);

        for(int i = 0; i < NUM_TILES; i++)
        {
          auto &src = m_model.tiles[i];
          commit(i, ParameterId::SampleFile, src.sample);
          commit(i, ParameterId::Pattern, src.pattern);
          commit(i, ParameterId::Balance, src.balance);
          commit(i, ParameterId::Gain, src.gain);
          commit(i, ParameterId::Mute, src.muted);
          commit(i, ParameterId::Reverse, src.reverse);
          commit(i, ParameterId::Selected, src.selected);
        }
      }

      void setParameter(TileId tileId, ParameterId parameterId, const ParameterValue &v) override
      {
        if(!tileId)
          setGlobalParameter(parameterId, v);
        else
          setTileParameter(m_model.tiles[tileId.value()], parameterId, v);

        m_dsp.takeAudioKernel(newDspKernel(m_model));
        commit(tileId, parameterId, v);
      }

      Dsp::SharedSampleBuffer getSamples(Computation *computation, TileId tileId) const override
      {
        return m_dsp.getSamples(get<std::filesystem::path>(getParameter(computation, tileId, ParameterId::SampleFile)));
      }

      Dsp::AudioKernel *newDspKernel(const DataModel &dataModel) const
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

      void translateGlobals(auto &target, const auto &source) const
      {
        auto numFramesPerMinute = SAMPLERATE * 60;
        auto num16thPerMinute = source.tempo * 4;

        target->framesPer16th = numFramesPerMinute / num16thPerMinute;
        target->volume = source.volume;
      }

      void translateTile(const DataModel &data, auto &tgt, const auto &src) const
      {
        tgt.pattern = src.pattern;
        tgt.audio = m_dsp.getSamples(src.sample);
        auto unbalancedGain = src.muted ? 0.f : src.gain;
        tgt.gainLeft = src.balance < 0 ? unbalancedGain : unbalancedGain * (1.0f - src.balance);
        tgt.gainRight = src.balance > 0 ? unbalancedGain : unbalancedGain * (1.0f + src.balance);
        tgt.playbackFrameIncrement = src.reverse ? -1 : 1;
      }

      void setGlobalParameter(const ParameterId &parameterId, const ParameterValue &v)
      {
        switch(parameterId)
        {
          case ParameterId::GlobalVolume:
            m_model.volume = std::clamp(std::get<Float>(v), 0.f, 1.f);
            break;

          case ParameterId::GlobalTempo:
            m_model.tempo = std::clamp(std::get<Float>(v), 20.f, 240.f);
            break;
        }
      }

      void setTileParameter(DataModel::Tile &tile, const ParameterId &parameterId, const ParameterValue &v)
      {
        switch(parameterId)
        {
          case ParameterId::SampleFile:
            tile.sample = std::get<Path>(v);
            break;

          case ParameterId::Pattern:
            tile.pattern = std::get<Pattern>(v);
            break;

          case ParameterId::Balance:
            tile.balance = std::get<Float>(v);
            break;

          case ParameterId::Gain:
            tile.gain = std::get<Float>(v);
            break;

          case ParameterId::Mute:
            tile.muted = std::get<Bool>(v);
            break;

          case ParameterId::Reverse:
            tile.reverse = std::get<Bool>(v);
            break;

          case ParameterId::Selected:
            for(auto c = 0; c < NUM_TILES; c++)
            {
              auto &src = m_model.tiles[c];
              src.selected = (src.id == tile.id);
              commit(src.id, ParameterId::Selected, src.selected);
            }
            break;
        }
      }

     private:
      DataModel &m_model;
      Dsp::Api::Control::Interface &m_dsp;
    };
  }

  static std::filesystem::path getInitFileName()
  {
    std::filesystem::path home = getenv("HOME");
    return home / ".mosaik";
  }

  Core::Core(Dsp::Api::Control::Interface &dsp)
      : m_dsp(dsp)
      , m_dataModel(std::make_unique<DataModel>(getInitFileName()))
      , m_api(std::make_unique<Api::Mosaik>(*m_dataModel.get(), m_dsp))
  {
  }

  Core::~Core() = default;

  Api::Interface &Core::getApi() const
  {
    return *m_api;
  }
}