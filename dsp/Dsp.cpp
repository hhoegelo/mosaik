#include "Dsp.h"
#include "api/control/Interface.h"
#include "api/display/Interface.h"
#include "api/realtime/Interface.h"
#include "PointerExchange.h"
#include "tools/Tools.h"
#include <numeric>
#include <filesystem>

namespace Dsp
{
  struct Voice
  {
    struct Params
    {
      double volume = 1.0;   // 0 ... 1
      double balance = 0.0;  // -1 ... 1
    };

    using Sample = float;
    using Frame = std::tuple<Sample, Sample>;
    using Buffer = std::vector<StereoFrame>;

    PointerExchange<Params> params { new Params() };
    PointerExchange<Buffer> buffer { new Buffer() };

    uint64_t framePos = 0;

    StereoFrame doAudio()
    {
      auto params = this->params.get();
      auto buffer = this->buffer.get();

      if(framePos >= buffer->size())
        return {};

      return (*buffer)[framePos++];
    }
  };

  static StereoFrame operator+(const StereoFrame &lhs, const StereoFrame &rhs)
  {
    return { lhs.left + rhs.left, lhs.right + rhs.right };
  }

  static StereoFrame operator*(const StereoFrame &lhs, float f)
  {
    return { lhs.left * f, lhs.right * f };
  }

  struct Dsp::Impl
  {
    Impl()
        : audioKernel(new Api::Control::AudioKernel)
    {
    }

    uint8_t step = 0;
    uint32_t framesInCurrentStep = 0;
    float volume = 1.0f;

    struct Channel
    {
      float gainLeft { 1.0f };
      float gainRight { 1.0f };
      int64_t framePosition = 0;

      StereoFrame doAudio(Api::Control::AudioKernel::Channel &kernel, uint8_t lastStep, uint8_t currentStep)
      {
        if(kernel.audio->empty())
          return {};

        if(lastStep != currentStep && kernel.pattern[currentStep])
        {
          if(kernel.playbackFrameIncrement < 0)
            framePosition = kernel.audio->size() - 1;
          else if(kernel.playbackFrameIncrement > 0)
            framePosition = 0;
        }

        if(framePosition < 0 || kernel.audio->size() <= framePosition)
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

    std::array<Channel, NUM_CHANNELS> channels;

    PointerExchange<Api::Control::AudioKernel> audioKernel;

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

      for(auto c = 0; c < NUM_CHANNELS; c++)
        frame = frame + channels[c].doAudio(kernel->channels[c], lastStep, step);

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
          // get some values from m_dsp
          return Position::zero();
        }

       private:
        Dsp::Impl &m_dsp;
      };
    }

    namespace Realtime
    {
      StereoFrame operator+(const StereoFrame &a, const StereoFrame &b)
      {
        return { a.left + b.left, a.right + b.right };
      }

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
          {
            f.main = m_dsp.doAudio();
          }
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