#include "Dsp.h"
#include <dsp/Types.h>
#include "api/control/Interface.h"
#include "api/display/Interface.h"
#include "api/realtime/Interface.h"
#include "PointerExchange.h"
#include "core/Types.h"
#include "dsp/tools/Tools.h"
#include <dsp/AudioKernel.h>
#include <map>

namespace Dsp
{
  struct Dsp::Impl
  {
    Impl()
        : audioKernel(new AudioKernel)
    {
    }

    struct ToUi
    {
      Step currentStep = 0;

      struct Tile
      {
        float currentLevel { 0.0f };
      };

      std::array<Tile, NUM_TILES> tiles;
    };

    ToUi toUi;

    Step step { 0 };
    uint32_t framesInCurrentStep = 0;
    float volume = 1.0f;

    struct Tile
    {
      float gainLeft { 1.0f };
      float gainRight { 1.0f };
      int64_t framePosition = 0;
      bool virgin = true;

      StereoFrame doAudio(AudioKernel::Tile &kernel, ToUi::Tile &ui, uint8_t lastStep, uint8_t currentStep)
      {
        if(kernel.audio->empty())
        {
          ui.currentLevel = 0;
          return {};
        }

        if(lastStep != currentStep && kernel.pattern[currentStep])
        {
          virgin = false;
          if(kernel.playbackFrameIncrement < 0)
            framePosition = kernel.audio->size() - 1;
          else if(kernel.playbackFrameIncrement > 0)
            framePosition = 0;
        }

        if(virgin || framePosition < 0 || kernel.audio->size() <= framePosition)
        {
          ui.currentLevel = 0;
          return {};
        }

        constexpr auto maxVolStep = 1000.0f / SAMPLERATE;
        gainLeft += std::clamp(kernel.gainLeft - gainLeft, -maxVolStep, maxVolStep);
        gainRight += std::clamp(kernel.gainRight - gainRight, -maxVolStep, maxVolStep);

        auto r = kernel.audio->operator[](framePosition);
        r.left *= gainLeft;
        r.right *= gainRight;

        ui.currentLevel = std::max({ ui.currentLevel, std::abs(r.left), std::abs(r.right) });

        framePosition += kernel.playbackFrameIncrement;
        return r;
      }
    };

    std::array<Tile, NUM_TILES> tiles;

    PointerExchange<AudioKernel> audioKernel;

    StereoFrame doAudio()
    {
      constexpr auto maxVolStep = 1000.0f / SAMPLERATE;
      auto kernel = audioKernel.get();

      auto lastStep = step;

      if(++framesInCurrentStep >= kernel->framesPer16th)
      {
        step = (lastStep + 1) % NUM_STEPS;
        framesInCurrentStep = 0;
      }

      // do processing
      StereoFrame frame {};

      for(auto c = 0; c < NUM_TILES; c++)
        frame = frame + tiles[c].doAudio(kernel->tiles[c], toUi.tiles[c], lastStep, step);

      volume += std::clamp(kernel->volume - volume, -maxVolStep, maxVolStep);

      // update ui
      toUi.currentStep = step;

      return frame * volume;
    }
  };

  namespace Api
  {

    namespace Control
    {
      class Mosaik : public Interface
      {
       public:
        explicit Mosaik(Dsp::Impl &dsp)
            : m_dsp(dsp)
        {
        }

        ~Mosaik() override = default;

        void takeAudioKernel(AudioKernel *kernel) override
        {
          m_dsp.audioKernel.set(kernel);
        }

        SharedSampleBuffer getSamples(const std::filesystem::path &path) const override
        {
          auto it = m_sampleFileCache.find(path);

          if(it != m_sampleFileCache.end())
            return it->second;

          auto ret = std::make_shared<SampleBuffer>(Tools::loadFile(path));
          m_sampleFileCache[path] = ret;
          return ret;
        }

       private:
        Dsp::Impl &m_dsp;

        mutable std::map<std::filesystem::path, SharedSampleBuffer> m_sampleFileCache;
      };
    }

    namespace Display
    {
      class Mosaik : public Interface
      {
       public:
        explicit Mosaik(Dsp::Impl &dsp)
            : m_dsp(dsp)
        {
        }

        ~Mosaik() override = default;

        [[nodiscard]] Step getCurrentStep() const override
        {
          return m_dsp.toUi.currentStep;
        }

        [[nodiscard]] float getCurrentTileLevel(Core::TileId tileId) override
        {
          return std::exchange(m_dsp.toUi.tiles[tileId.value()].currentLevel, 0.f);
        }

       private:
        Dsp::Impl &m_dsp;
      };
    }

    namespace Realtime
    {
      class Mosaik : public Interface
      {
       public:
        explicit Mosaik(Dsp::Impl &dsp)
            : m_dsp(dsp)
        {
        }

        ~Mosaik() override = default;

        void doAudio(OutFrame *out, size_t numFrames, const SendMidi &cb) override
        {
          for(size_t i = 0; i < numFrames; i++)
            out[i].main = m_dsp.doAudio();
        }

        void doMidi(const MidiEvent &inEvent) override
        {
          // handle midi events here
        }

       private:
        Dsp::Impl &m_dsp;
      };
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