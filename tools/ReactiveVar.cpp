#include "ReactiveVar.h"

#include <utility>
#include <glibmm/main.h>
#include <cassert>

namespace Tools
{
  thread_local std::shared_ptr<Computations::Computation> tl_currentComputation;

  struct Computations::Computation : std::enable_shared_from_this<Computation>
  {
    using Callback = std::function<void()>;

    Computation(Computations &owner, Callback cb)
        : m_owner(owner)
        , m_cb(std::move(cb))
    {
    }

    void execute()
    {
      auto pThis = shared_from_this();
      std::swap(tl_currentComputation, pThis);
      m_cb();
      std::swap(tl_currentComputation, pThis);
    }

    void invalidate()
    {
      m_owner.onComputationInvalidated(this);
    }

    Computations &m_owner;
    Callback m_cb;
  };

  ReactiveVarBase::ReactiveVarBase() = default;

  ReactiveVarBase::~ReactiveVarBase() = default;

  void ReactiveVarBase::registerCurrentComputation() const
  {
    if(tl_currentComputation)
      m_computations.insert(tl_currentComputation);

    for(auto it = m_computations.begin(); it != m_computations.end();)
    {
      if(!it->lock())
        it = m_computations.erase(it);
      else
        it++;
    }
  }

  void ReactiveVarBase::invalidate()
  {
    auto computations = std::move(m_computations);

    for(auto &computation : computations)
      if(auto c = computation.lock())
        c->invalidate();
  }

  Computations::~Computations() = default;

  void Computations::add(const std::function<void()> &cb)
  {
    auto p = std::make_shared<Computation>(*this, cb);
    m_computations.push_back(p);
    p->execute();
  }

  void ImmediateComputations::onComputationInvalidated(Computations::Computation *c)
  {
    auto cb = std::move(c->m_cb);
    m_computations.erase(
        std::find_if(m_computations.begin(), m_computations.end(), [&](auto &m) { return m.get() == c; }));
    add(cb);
  }

  uint32_t DeferredComputations::s_numDeferredComputationsScheduled = 0;

  DeferredComputations::DeferredComputations()
      : DeferredComputations(Glib::MainContext::get_default(), 10)
  {
  }

  DeferredComputations::DeferredComputations(Glib::RefPtr<Glib::MainContext> ctx, uint32_t timeout)
      : m_ctx(std::move(ctx))
      , m_timeout(timeout)
  {
  }

  void DeferredComputations::onComputationInvalidated(Computations::Computation *c)
  {
    m_pending.push_back(c->m_cb);

    m_computations.erase(
        std::find_if(m_computations.begin(), m_computations.end(), [&](auto &m) { return m.get() == c; }));

    if(m_pending.size() == 1)
    {
      s_numDeferredComputationsScheduled++;

      m_timer = m_ctx->signal_timeout().connect(
          [this]
          {
            auto c = std::move(m_pending);
            for(const auto &k : c)
              add(k);
            s_numDeferredComputationsScheduled--;
            return false;
          },
          m_timeout);
    }
  }

  DeferredComputations::~DeferredComputations()
  {
    m_timer.disconnect();
    m_initTimer.disconnect();
  }

  void DeferredComputations::waitForAllScheduledComputationsDone()
  {
    while(s_numDeferredComputationsScheduled > 0)
      Glib::MainContext::get_default()->iteration(true);
  }

}
