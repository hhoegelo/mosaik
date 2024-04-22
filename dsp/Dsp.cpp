#include "Dsp.h"
#include <dsp/Types.h>
#include "dsp/tools/AudioFileDecoder.h"

#include "api/control/Interface.h"
#include "api/display/Interface.h"
#include "api/realtime/Interface.h"
#include "PointerExchange.h"
#include <dsp/AudioKernel.h>
#include <tools/ReactiveVar.h>
#include <map>

#include <dsp/Mosaik.h>

namespace Dsp
{

  namespace Api
  {
    namespace Control
    {
      class Mosaik : public Interface
      {
       public:
        explicit Mosaik(::Dsp::Mosaik &dsp);
        ~Mosaik() override = default;
        void takeAudioKernel(AudioKernel *kernel) override;
        SharedSampleBuffer getSamples(const std::filesystem::path &path) const override;
        void cleanupCache(std::vector<std::filesystem::path> &&paths) override;

       private:
        ::Dsp::Mosaik &m_dsp;

        mutable std::map<std::filesystem::path, ::Tools::ReactiveVar<SharedSampleBuffer>> m_sampleFileCache;
        mutable std::vector<std::unique_ptr<Tools::AudioFileDecoder>> m_fetchers;
      };

      Mosaik::Mosaik(::Dsp::Mosaik &dsp)
          : m_dsp(dsp)
      {
      }

      void Mosaik::takeAudioKernel(AudioKernel *kernel)
      {
        m_dsp.set(kernel);
      }

      SharedSampleBuffer Mosaik::getSamples(const std::filesystem::path &path) const
      {
        auto it = m_sampleFileCache.find(path);

        if(it != m_sampleFileCache.end())
          return it->second;

        m_sampleFileCache[path] = std::make_shared<SampleBuffer>();

        try
        {
          m_fetchers.push_back(std::make_unique<Tools::AudioFileDecoder>(
              path,
              [this, path](auto &&buffer)
              {
                if(buffer.has_value())
                  m_sampleFileCache[path] = std::make_shared<SampleBuffer>(std::move(buffer.value()));

                auto newEnd = std::remove_if(m_fetchers.begin(), m_fetchers.end(), [](auto &f) { return f->isDone(); });
                m_fetchers.erase(newEnd, m_fetchers.end());
              }));
        }
        catch(...)
        {
        }

        return m_sampleFileCache[path];
      }

      void Mosaik::cleanupCache(std::vector<std::filesystem::path> &&paths)
      {
        for(auto it = m_sampleFileCache.begin(); it != m_sampleFileCache.end();)
        {
          if(!std::count(paths.begin(), paths.end(), it->first))
          {
            it = m_sampleFileCache.erase(it);
          }
          else
          {
            it++;
          }
        }
      }
    }

    namespace Display
    {
      class Mosaik : public Interface
      {
       public:
        Mosaik(::Dsp::Mosaik &dsp, Control::Interface &ctrl);
        ~Mosaik() override = default;
        [[nodiscard]] FramePos getCurrentLoopPosition() const override;
        [[nodiscard]] std::tuple<float, float> getLevel(Core::TileId tileId) override;
        [[nodiscard]] FramePos getPosition(Core::TileId tileId) override;
        [[nodiscard]] std::chrono::milliseconds getDuration(const std::filesystem::path &file) const override;

       private:
        ::Dsp::Mosaik &m_dsp;
        Control::Interface &m_ctrl;
      };

      Mosaik::Mosaik(::Dsp::Mosaik &dsp, Control::Interface &ctrl)
          : m_dsp(dsp)
          , m_ctrl(ctrl)
      {
      }

      FramePos Mosaik::getCurrentLoopPosition() const
      {
        return m_dsp.getUiInfo().currentLoopPosition;
      }

      std::tuple<float, float> Mosaik::getLevel(Core::TileId tileId)
      {
        return { std::exchange(m_dsp.getUiInfo().tiles[tileId.value()].levelLeft, 0.f),
                 std::exchange(m_dsp.getUiInfo().tiles[tileId.value()].levelRight, 0.f) };
      }

      std::chrono::milliseconds Mosaik::getDuration(const std::filesystem::path &file) const
      {
        return std::chrono::milliseconds(1000 * m_ctrl.getSamples(file)->size() / SAMPLERATE);
      }

      FramePos Mosaik::getPosition(Core::TileId tileId)
      {
        return m_dsp.getUiInfo().tiles[tileId.value()].frame;
      }
    }

    namespace Realtime
    {
      class Mosaik : public Interface
      {
       public:
        explicit Mosaik(::Dsp::Mosaik &dsp);

        ~Mosaik() override = default;
        void doAudio(OutFrame *out, size_t numFrames, const SendMidi &cb) override;
        void doMidi(const MidiEvent &inEvent) override;

       private:
        ::Dsp::Mosaik &m_dsp;
      };

      Mosaik::Mosaik(::Dsp::Mosaik &dsp)
          : m_dsp(dsp)
      {
      }

      void Mosaik::doAudio(OutFrame *out, size_t numFrames, const Interface::SendMidi &cb)
      {
        for(size_t i = 0; i < numFrames; i++)
          out[i] = m_dsp.doAudio();
      }

      void Mosaik::doMidi(const MidiEvent &inEvent)
      {
        // handle midi events here
      }
    }
  }

  Dsp::Dsp()
      : m_impl(std::make_unique<Mosaik>())
      , m_controlApi(std::make_unique<Api::Control::Mosaik>(*m_impl))
      , m_displayApi(std::make_unique<Api::Display::Mosaik>(*m_impl, *m_controlApi))
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