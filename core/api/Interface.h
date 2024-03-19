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
    Computation() = default;
    Computation(Computation &&) = delete;

    explicit Computation(std::function<void(Computation *)> cb)
        : m_cb(std::move(cb))
    {
      m_cb(this);
    }

    ~Computation()
    {
      m_timer.disconnect();
    }

    template <typename... Args> void add(const Tools::Signals::Signal<Args...> *s)
    {
      auto c = const_cast<Tools::Signals::Signal<Args...> *>(s)->connectWithoutInit(
          [this](auto...)
          {
            if(!std::exchange(m_timerScheduled, true))
            {
              m_timer = Glib::MainContext::get_default()->signal_timeout().connect(
                  [this]
                  {
                    m_timerScheduled = false;
                    if(m_cb)
                    {
                      m_connections.clear();
                      m_cb(this);
                    }
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
    std::function<void(Computation *)> m_cb;
  };

  class Computations
  {
   public:
    template <typename T> void add(T &&f)
    {
      m_computations.push_back(std::make_unique<Computation>(f));
    }

   private:
    std::vector<std::unique_ptr<Computation>> m_computations;
  };

  class Interface
  {
   public:
    virtual ~Interface() = default;

    virtual void setParameter(TileId tileId, ParameterId parameterId, const ParameterValue &value) = 0;
    virtual void incParameter(TileId tileId, ParameterId parameterId, int steps) = 0;
    virtual Dsp::SharedSampleBuffer getSamples(Computation *computation, TileId tileId) const = 0;

    ParameterValue getParameter(Computation *computation, TileId tileId, ParameterId parameterId) const;
    Tools::Signals::Connection connect(TileId tileId, ParameterId id,
                                       const std::function<void(const ParameterValue &)> &cb);
    std::vector<TileId> getSelectedTiles(Computation *computation) const;

    // Convenience
    Step loopPositionToStep(Dsp::FramePos pos) const;
    Pattern getMergedPattern(Computation *computation) const;
    ParameterValue getFirstSelectedTileParameter(Computation *computation, ParameterId id) const;
    void incSelectedTilesParameter(ParameterId parameterId, int steps);
    void toggleSelectedTilesParameter(ParameterId parameterId);

    void setStep(Step step, bool value);

   protected:
    void commit(TileId tileId, ParameterId parameterId, const ParameterValue &v);

   private:
    std::map<std::tuple<TileId, ParameterId>, Tools::Signals::Signal<ParameterValue>> m_parameterCache;
  };

  template <typename... Args>
  std::tuple_element_t<0, std::tuple<Args...>> connect(Computation *c, const Tools::Signals::Signal<Args...> &s)
  {
    if(c)
      c->add(&s);
    return std::get<0>(s.getCache());
  }
}
