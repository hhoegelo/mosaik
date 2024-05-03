#pragma once

#include <glibmm/refptr.h>
#include <glibmm/main.h>

#include <utility>
#include <memory>
#include <set>
#include <algorithm>
#include <functional>
#include <vector>

namespace Tools
{
  class Computations
  {
   public:
    class Computation;

    virtual ~Computations();
    virtual void add(const std::function<void()>& cb, bool fromOutside = true);

   protected:
    virtual void onComputationInvalidated(Computation* c) = 0;
    std::vector<std::shared_ptr<Computation>> m_computations;
  };

  class ImmediateComputations : public Computations
  {
   private:
    void onComputationInvalidated(Computation* c) override;
  };

  class DeferredComputations : public Computations
  {
   public:
    DeferredComputations();
    explicit DeferredComputations(Glib::RefPtr<Glib::MainContext> ctx, uint32_t timeout = 10);
    ~DeferredComputations() override;

    void add(const std::function<void()>& cb, bool fromOutside = true) override;

    static void waitForAllScheduledComputationsDone();

   private:
    void onComputationInvalidated(Computation* c) override;

    Glib::RefPtr<Glib::MainContext> m_ctx;
    uint32_t m_timeout;
    sigc::connection m_timer;
    sigc::connection m_initTimer;
    std::vector<std::function<void()>> m_pending;
    static uint32_t s_numDeferredComputationsScheduled;
  };

  class ReactiveVarBase
  {
   public:
    ReactiveVarBase();
    virtual ~ReactiveVarBase();

   protected:
    void registerCurrentComputation() const;
    void invalidate();

    mutable std::set<std::weak_ptr<Computations::Computation>, std::owner_less<>> m_computations;
  };

  template <typename T> class ReactiveVar : public ReactiveVarBase
  {
   public:
    ReactiveVar(const T& init = {})
        : m_value(init)
    {
    }

    ~ReactiveVar() override = default;

    ReactiveVar<T>& operator=(const T& v)
    {
      if(std::exchange(m_value, v) != v)
        invalidate();
      return *this;
    }

    explicit operator const T&() const
    {
      registerCurrentComputation();
      return m_value;
    }

    operator T() const  // NOLINT
    {
      registerCurrentComputation();
      return m_value;
    }

    const T& get() const
    {
      registerCurrentComputation();
      return m_value;
    }

   private:
    T m_value;
  };

}