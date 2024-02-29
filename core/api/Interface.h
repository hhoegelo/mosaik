#pragma once

#include <core/Types.h>
#include <dsp/Types.h>
#include <map>
#include <sigc++/sigc++.h>
#include "tools/SignalSlot.h"
#include <utility>
#include <glibmm.h>

namespace Core::Api
{
  using Step = uint8_t;
  
  class Computation
  {
   public:
    ~Computation()
    {
      m_timer.disconnect();
    }

    void refresh(std::function<void()> cb)
    {
      m_cb = std::move(cb);
    }

    template <typename Signature> void add(const Tools::Signals::Signal<void(Signature)> *s)
    {
      auto c = const_cast<Tools::Signals::Signal<void(Signature)> *>(s)->connect(
          [this](Signature)
          {
            if(!std::exchange(m_timerScheduled, true))
            {
              m_timer = Glib::MainContext::get_default()->signal_timeout().connect(
                  [this]
                  {
                    m_timerScheduled = false;
                    if(m_cb)
                      m_cb();
                    return false;
                  },
                  25);
            }
          });
      m_connections[(void *) s] = c;
    }

   private:
    sigc::connection m_timer;
    bool m_timerScheduled = false;
    std::map<void *, Tools::Signals::Connection> m_connections;
    std::function<void()> m_cb;
  };

  namespace Detail
  {
    template <typename T> struct SignalingCache
    {
      T cache;
      Tools::Signals::Signal<void(T)> sig;
    };
  }

  class Interface
  {
   public:
    virtual ~Interface() = default;

    virtual void setParameter(TileId tileId, ParameterId parameterId, const ParameterValue &value) = 0;
    virtual Dsp::SharedSampleBuffer getSamples(Computation *computation, TileId tileId) const = 0;

    ParameterValue getParameter(Computation *computation, TileId tileId, ParameterId parameterId) const;
    Tools::Signals::Connection connect(TileId tileId, ParameterId id,
                                       const std::function<void(const ParameterValue &)> &cb);
    std::vector<TileId> getSelectedTiles(Computation *computation) const;

    // Convenience
    Pattern getMergedPattern(Computation *computation) const;
    void setStep(Step step, bool value);

   protected:
    void commit(TileId tileId, ParameterId parameterId, const ParameterValue &v);

   private:
    std::map<std::tuple<TileId, ParameterId>, Detail::SignalingCache<ParameterValue>> m_parameterCache;
  };
}
