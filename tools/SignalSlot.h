#pragma once

#include <memory>
#include <functional>
#include <algorithm>

namespace TOOLS
{
  namespace SIGNALS
  {
    class Connection
    {
     public:
     private:
      std::shared_ptr<void> m_ptr;
    };

    template <typename... Args> class Signal
    {
     public:
      using Callback = std::function<void(Args &&...)>;

      void send(Args &&...args)
      {
        auto newEnd = std::remove_if(m_callbacks.begin(), m_callbacks.end(),
                                     [&args...](auto &cb)
                                     {
                                       if(auto locked = cb.lock())
                                       {
                                         locked(args...);
                                         return false;
                                       }
                                       return true;
                                     });
        m_callbacks.erase(newEnd, m_callbacks.end());
      }

      Connection connect(const Callback &cb)
      {
        m_callbacks.push_back(Connection(cb));
        return m_callbacks.back();
      }

     private:
      std::vector<std::weak_ptr<Callback>> m_callbacks;
    };
  }
}