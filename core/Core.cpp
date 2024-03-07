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
        // Globals
        commit({}, ParameterId::GlobalTempo, m_model.globals.tempo);
        commit({}, ParameterId::GlobalVolume, m_model.globals.volume);

        // Tiles
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
          commit(tileId, parameterId, setGlobalParameter(parameterId, v));
        else
          commit(tileId, parameterId, setTileParameter(m_model.tiles[tileId.value()], parameterId, v));

        m_dsp.takeAudioKernel(newDspKernel(m_model));
      }

      void incParameter(TileId tileId, ParameterId parameterId, int steps) override
      {
        switch(parameterId)
        {
          case ParameterId::GlobalTempo:
            setParameter(tileId, parameterId,
                         std::get<float>(getParameter(nullptr, tileId, parameterId)) + static_cast<float>(steps));
            break;

          case ParameterId::GlobalVolume:
          case ParameterId::Gain:
          case ParameterId::Balance:
            setParameter(tileId, parameterId,
                         std::get<float>(getParameter(nullptr, tileId, parameterId))
                             + static_cast<float>(steps) / 100.0f);
            break;

          case ParameterId::Selected:
          case ParameterId::SampleFile:
          case ParameterId::Pattern:
          case ParameterId::Mute:
          case ParameterId::Reverse:
            break;
        }
      }

      Dsp::SharedSampleBuffer getSamples(Computation *computation, TileId tileId) const override
      {
        return m_dsp.getSamples(get<std::filesystem::path>(getParameter(computation, tileId, ParameterId::SampleFile)));
      }

      [[nodiscard]] Dsp::AudioKernel *newDspKernel(const DataModel &dataModel) const
      {
        auto r = std::make_unique<Dsp::AudioKernel>();
        translateGlobals(r, dataModel);

        for(auto c = 0; c < NUM_TILES; c++)
        {
          const auto &src = dataModel.tiles[c];
          auto &tgt = r->tiles[c];
          translateTile(tgt, src);
        }

        return r.release();
      }

      void translateGlobals(auto &target, const auto &source) const
      {
        auto numFramesPerMinute = SAMPLERATE * 60;
        auto num16thPerMinute = source.globals.tempo * 4;

        target->framesPer16th = numFramesPerMinute / num16thPerMinute;
        target->volume = source.globals.volume;
      }

      void translateTile(auto &tgt, const auto &src) const
      {
        tgt.pattern = src.pattern;
        tgt.audio = m_dsp.getSamples(src.sample);
        auto unbalancedGain = src.muted ? 0.f : src.gain;
        tgt.gainLeft = src.balance < 0 ? unbalancedGain : unbalancedGain * (1.0f - src.balance);
        tgt.gainRight = src.balance > 0 ? unbalancedGain : unbalancedGain * (1.0f + src.balance);
        tgt.playbackFrameIncrement = src.reverse ? -1 : 1;
      }

      ParameterValue setGlobalParameter(const ParameterId &parameterId, const ParameterValue &v)
      {
        switch(parameterId)
        {
          case ParameterId::GlobalVolume:
            m_model.globals.volume = std::clamp(std::get<Float>(v), 0.f, 1.f);
            return m_model.globals.volume;

          case ParameterId::GlobalTempo:
            m_model.globals.tempo = std::clamp(std::get<Float>(v), 20.f, 240.f);
            return m_model.globals.tempo;

          default:
            throw std::runtime_error("unhandled global parameter");
        }
      }

      ParameterValue setTileParameter(DataModel::Tile &tile, const ParameterId &parameterId, const ParameterValue &v)
      {
        switch(parameterId)
        {
          case ParameterId::SampleFile:
            tile.sample = std::get<Path>(v);
            return tile.sample;

          case ParameterId::Pattern:
            tile.pattern = std::get<Pattern>(v);
            return tile.pattern;

          case ParameterId::Balance:
            tile.balance = std::get<Float>(v);
            return tile.balance;

          case ParameterId::Gain:
            tile.gain = std::get<Float>(v);
            return tile.gain;

          case ParameterId::Mute:
            tile.muted = std::get<Bool>(v);
            return tile.muted;

          case ParameterId::Reverse:
            tile.reverse = std::get<Bool>(v);
            return tile.reverse;

          case ParameterId::Selected:
            for(auto c = 0; c < NUM_TILES; c++)
            {
              auto &src = m_model.tiles[c];
              src.selected = (src.id == tile.id);
              commit(src.id, ParameterId::Selected, src.selected);
            }
            return true;

          default:
            throw std::runtime_error("unhandled tile parameter");
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
      , m_api(std::make_unique<Api::Mosaik>(*m_dataModel, m_dsp))
  {
  }

  Core::~Core() = default;

  Api::Interface &Core::getApi() const
  {
    return *m_api;
  }
}