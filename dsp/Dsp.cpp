#include "Dsp.h"
#include <dsp/Types.h>
#include <tools/Math.h>
#include "api/control/Interface.h"
#include "api/display/Interface.h"
#include "api/realtime/Interface.h"
#include "PointerExchange.h"
#include "core/Types.h"
#include "dsp/tools/Tools.h"
#include <dsp/AudioKernel.h>
#include <map>
#include <cmath>

namespace Dsp
{
  constexpr int c_silenceDB = -60;
  constexpr int c_maxDB = 12;

  struct Dsp::Impl
  {
    Impl()
        : audioKernel(new AudioKernel)
    {
    }

    struct ToUi
    {
      FramePos currentLoopPosition = 0;

      struct Tile
      {
        float currentLevel { 0.0f };
      };

      std::array<Tile, NUM_TILES> tiles;
    };

    ToUi toUi;

    Step step { 0 };
    FramePos position = 0;
    FramePos knownFramesPerLoop = 0;
    float volume = 1.0f;

    struct Tile
    {
      static constexpr auto invalidFramePosF32 = std::numeric_limits<float>::max();
      static constexpr auto invalidFramePosU64 = std::numeric_limits<FramePos>::max();
      float gainLeft { 1.0f };
      float gainRight { 1.0f };
      float framePosition = invalidFramePosF32;
      StereoFrame doAudio(AudioKernel::Tile &kernel, ToUi::Tile &ui, FramePos currentLoopPosition);
      float doEnvelope(AudioKernel::Tile &kernel, FramePos iFramePos) const;
    };

    std::array<Tile, NUM_TILES> tiles;

    PointerExchange<AudioKernel> audioKernel;

    StereoFrame doAudio();
  };

  StereoFrame Dsp::Impl::doAudio()
  {
    constexpr auto maxVolStep = 1000.0f / SAMPLERATE;
    auto kernel = audioKernel.get();

    if(!knownFramesPerLoop)
      knownFramesPerLoop = kernel->framesPerLoop;

    if(knownFramesPerLoop != kernel->framesPerLoop)
    {
      auto oldLoopPositionRel
          = static_cast<double>(position % knownFramesPerLoop) / static_cast<double>(knownFramesPerLoop);

      position = static_cast<FramePos>(oldLoopPositionRel * static_cast<double>(kernel->framesPerLoop));
    }

    knownFramesPerLoop = kernel->framesPerLoop;

    auto currentLoopPosition = (position++ % kernel->framesPerLoop);
    toUi.currentLoopPosition = currentLoopPosition;

    // do processing
    StereoFrame frame {};

    for(auto c = 0; c < NUM_TILES; c++)
      frame = frame + tiles[c].doAudio(kernel->tiles[c], toUi.tiles[c], currentLoopPosition);

    volume
        += std::clamp(::Tools::dBToFactor<c_silenceDB, c_maxDB>(kernel->volume_dB) - volume, -maxVolStep, maxVolStep);

    return frame;
  }

  StereoFrame Dsp::Impl::Tile::doAudio(AudioKernel::Tile &kernel, Dsp::Impl::ToUi::Tile &ui,
                                       FramePos currentLoopPosition)
  {
    const auto &audio = *kernel.audio;

    if(std::binary_search(kernel.triggers.begin(), kernel.triggers.end(), currentLoopPosition))
      framePosition = 0;

    StereoFrame result = {};

    auto iFramePos
        = invalidFramePosF32 == framePosition ? invalidFramePosU64 : static_cast<FramePos>(std::round(framePosition));

    if(iFramePos < audio.size())
    {
      if(kernel.reverse)
        result = audio[audio.size() - 1 - iFramePos];
      else
        result = audio[iFramePos];
    }

    constexpr auto maxVolStep = 10000.0f / SAMPLERATE;

    auto target_dB = kernel.mute ? c_silenceDB : kernel.gain_dB + doEnvelope(kernel, iFramePos);
    auto targetGain = ::Tools::dBToFactor<c_silenceDB, c_maxDB>(target_dB);
    auto targetGainLeft = (kernel.balance < 0) ? targetGain : targetGain * (1.0f - kernel.balance);
    auto targetGainRight = (kernel.balance > 0) ? targetGain : targetGain * (1.0f + kernel.balance);

    gainLeft += std::clamp(targetGainLeft - gainLeft, -maxVolStep, maxVolStep);
    gainRight += std::clamp(targetGainRight - gainRight, -maxVolStep, maxVolStep);

    result.left *= gainLeft;
    result.right *= gainRight;

    ui.currentLevel = std::max({ ui.currentLevel, std::abs(result.left), std::abs(result.right) });
    framePosition += framePosition != invalidFramePosF32 ? kernel.playbackFrameIncrement : 0;
    return result;
  }

  float Dsp::Impl::Tile::doEnvelope(AudioKernel::Tile &kernel, FramePos iFramePos) const
  {
    if(iFramePos != invalidFramePosU64)
    {
      for(auto &section : kernel.envelope)
      {
        if(iFramePos >= section.pos)
        {
          return section.b + framePosition * section.m;
        }
      }
    }
    return c_silenceDB;
  }

  namespace Api
  {
    namespace Control
    {
      class Mosaik : public Interface
      {
       public:
        explicit Mosaik(Dsp::Impl &dsp);
        ~Mosaik() override = default;
        void takeAudioKernel(AudioKernel *kernel) override;
        SharedSampleBuffer getSamples(const std::filesystem::path &path) const override;

       private:
        Dsp::Impl &m_dsp;

        mutable std::map<std::filesystem::path, SharedSampleBuffer> m_sampleFileCache;
      };

      Mosaik::Mosaik(Dsp::Impl &dsp)
          : m_dsp(dsp)
      {
      }

      void Mosaik::takeAudioKernel(AudioKernel *kernel)
      {
        m_dsp.audioKernel.set(kernel);
      }

      SharedSampleBuffer Mosaik::getSamples(const std::filesystem::path &path) const
      {
        auto it = m_sampleFileCache.find(path);

        if(it != m_sampleFileCache.end())
          return it->second;

        auto ret = std::make_shared<SampleBuffer>(Tools::loadFile(path));
        m_sampleFileCache[path] = ret;
        return ret;
      }
    }

    namespace Display
    {
      class Mosaik : public Interface
      {
       public:
        explicit Mosaik(Dsp::Impl &dsp);
        ~Mosaik() override = default;
        [[nodiscard]] FramePos getCurrentLoopPosition() const override;
        [[nodiscard]] float getCurrentTileLevel(Core::TileId tileId) override;

       private:
        Dsp::Impl &m_dsp;
      };

      Mosaik::Mosaik(Dsp::Impl &dsp)
          : m_dsp(dsp)
      {
      }

      FramePos Mosaik::getCurrentLoopPosition() const
      {
        return m_dsp.toUi.currentLoopPosition;
      }

      float Mosaik::getCurrentTileLevel(Core::TileId tileId)
      {
        return std::exchange(m_dsp.toUi.tiles[tileId.value()].currentLevel, 0.f);
      }
    }

    namespace Realtime
    {
      class Mosaik : public Interface
      {
       public:
        explicit Mosaik(Dsp::Impl &dsp);

        ~Mosaik() override = default;
        void doAudio(OutFrame *out, size_t numFrames, const SendMidi &cb) override;
        void doMidi(const MidiEvent &inEvent) override;

       private:
        Dsp::Impl &m_dsp;
      };

      Mosaik::Mosaik(Dsp::Impl &dsp)
          : m_dsp(dsp)
      {
      }

      void Mosaik::doAudio(OutFrame *out, size_t numFrames, const Interface::SendMidi &cb)
      {
        for(size_t i = 0; i < numFrames; i++)
          out[i].main = m_dsp.doAudio();
      }

      void Mosaik::doMidi(const MidiEvent &inEvent)
      {
        // handle midi events here
      }
    }
  }

  Dsp::Dsp()
      : m_impl(std::make_unique<Impl>())
      , m_controlApi(std::make_unique<Api::Control::Mosaik>(*m_impl))
      , m_displayApi(std::make_unique<Api::Display::Mosaik>(*m_impl))
      , m_realtimeApi(std::make_unique<Api::Realtime::Mosaik>(*m_impl))
  {
  }

  Dsp::~Dsp() = default;

  Api::Control::Interface &Dsp::getControlApi() const
  {
    return *m_controlApi;
  }

  Api::Realtime::Interface &Dsp::getRealtimeApi() const
  {
    return *m_realtimeApi;
  }

  Api::Display::Interface &Dsp::getDisplayApi() const
  {
    return *m_displayApi;
  }
}