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

  struct Dsp::Impl
  {
    std::array<Voice, 256> voices;
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

        void loadSample(Col col, Row row, const std::filesystem::path &path) override
        {
          m_dsp.voices[col + 16 * row].buffer.set(Tools::loadFile(path, m_samplerate));
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
            f.main = std::accumulate(m_dsp.voices.begin(), m_dsp.voices.end(), StereoFrame {},
                                     [](const StereoFrame &a, Voice &v) { return a + v.doAudio(); });
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