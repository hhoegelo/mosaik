#pragma once

#include <atomic>

template <typename T> class PointerExchange
{
 public:
  PointerExchange(T *p)
      : current(p)
  {
  }

  ~PointerExchange()
  {
    if(auto d = next.exchange(nullptr))
      delete d;

    if(auto d = current.exchange(nullptr))
      delete d;

    if(auto d = doomed.exchange(nullptr))
      delete d;
  }

  // to be called from UI thread
  inline void set(T *p)
  {
    if(auto d = doomed.exchange(nullptr))
      delete d;

    if(auto oldNext = next.exchange(p))
      delete oldNext;
  }

  // to be called from audio thread
  inline T *get()
  {
    if(!doomed)
    {
      if(auto n = next.exchange(nullptr))
      {
        doomed = current.exchange(n);
      }
    }
    return current;
  }

 private:
  std::atomic<T *> next;
  std::atomic<T *> current;
  std::atomic<T *> doomed;
};