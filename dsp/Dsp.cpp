#include "Dsp.h"
#include "api/control/Interface.h"
#include "api/display/Interface.h"
#include "api/realtime/Interface.h"

namespace Dsp
{
  struct Dsp::Impl
  {
    // here  goes all the stuff that you need for processing the audio
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

        void updateSomeChannelParameter(Col col, Row row, float v) override
        {
          // this fiddles with m_dsp members directly
        }

        void alsoDoSomethingElseForTheWholeColumn(Col col) override
        {
          // this fiddles with m_dsp members directly
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
          // here goes the audio loop that iterates m_dsp's members etc
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