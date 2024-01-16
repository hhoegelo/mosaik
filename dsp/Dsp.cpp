#include "Dsp.h"
#include "api/control/Interface.h"
#include "api/display/Interface.h"
#include "api/realtime/Interface.h"
#include "PointerExchange.h"
#include <dsp/AudioKernel.h>

namespace Dsp
{
  struct Dsp::Impl
  {
    Impl()
        : audioKernel(new AudioKernel)
    {
    }

    uint8_t step = 0;
    uint32_t framesInCurrentStep = 0;
    float volume = 1.0f;

    struct Tile
    {
      float gainLeft { 1.0f };
      float gainRight { 1.0f };
      int64_t framePosition = 0;
      bool virgin = true;

      StereoFrame doAudio(AudioKernel::Tile &kernel, uint8_t lastStep, uint8_t currentStep)
      {
        if(kernel.audio->empty())
          return {};

        if(lastStep != currentStep && kernel.pattern[currentStep])
        {
          virgin = false;
          if(kernel.playbackFrameIncrement < 0)
            framePosition = kernel.audio->size() - 1;
          else if(kernel.playbackFrameIncrement > 0)
            framePosition = 0;
        }

        if(virgin || framePosition < 0 || kernel.audio->size() <= framePosition)
          return {};

        constexpr auto maxVolStep = 1000.0f / SAMPLERATE;
        gainLeft += std::clamp(kernel.gainLeft - gainLeft, -maxVolStep, maxVolStep);
        gainRight += std::clamp(kernel.gainRight - gainRight, -maxVolStep, maxVolStep);

        auto r = kernel.audio->operator[](framePosition);
        r.left *= gainLeft;
        r.right *= gainRight;

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
        step = (step + 1) % NUM_STEPS;
        framesInCurrentStep = 0;
      }

      StereoFrame frame {};

      for(auto c = 0; c < NUM_TILES; c++)
        frame = frame + tiles[c].doAudio(kernel->tiles[c], lastStep, step);

      volume += std::clamp(kernel->volume - volume, -maxVolStep, maxVolStep);
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
        Mosaik(Dsp::Impl &dsp)
            : m_dsp(dsp)
        {
        }

        ~Mosaik() = default;

        void takeAudioKernel(AudioKernel *kernel) override
        {
          m_dsp.audioKernel.set(kernel);
        }

       private:
        Dsp::Impl &m_dsp;
      };
    }

    namespace Display
    {
      class Mosaik : public Interface
      {
       public:
        Mosaik(Dsp::Impl &dsp)
            : m_dsp(dsp)
        {
        }

        ~Mosaik() = default;

        Position getCurrentPosition() const override
        {
          return Position::zero();
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
        Mosaik(Dsp::Impl &dsp)
            : m_dsp(dsp)
        {
        }

        ~Mosaik() = default;

        void doAudio(const std::span<OutFrame> &out, const SendMidi &cb) override
        {
          for(auto &f : out)
            f.main = m_dsp.doAudio();
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
      , m_controlApi(std::make_unique<Api::Control::Mosaik>(*m_impl.get()))
      , m_displayApi(std::make_unique<Api::Display::Mosaik>(*m_impl.get()))
      , m_realtimeApi(std::make_unique<Api::Realtime::Mosaik>(*m_impl.get()))
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