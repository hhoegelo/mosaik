#pragma once

#include <memory>
#include <functional>
#include <algorithm>
#include <utility>

namespace Tools
{
  namespace Signals
  {
    namespace Detail
    {
      class ConnectionBase
      {
       public:
        virtual ~ConnectionBase() = default;
      };

      template <typename... Args> class Connection : public ConnectionBase
      {
       public:
        using Callback = std::function<void(Args...)>;
        explicit Connection(const Callback &cb)
            : m_cb(cb)
        {
        }

        Callback m_cb;
      };
    }

    using Connection = std::shared_ptr<Detail::ConnectionBase>;

    template <typename... Args> class Signal
    {
     public:
      using Callback = std::function<void(Args...)>;

      bool setCache(const Args &...args)
      {
        auto n = std::make_tuple(args...);
        return std::exchange(m_cache, n) != n;
      }

      void emit(const Args &...args)
      {
        auto changed = setCache(args...);
        auto hasContent = std::tuple_size_v<decltype(m_cache)> > 0;

        if(!hasContent || changed)
        {
          auto newEnd = std::remove_if(m_callbacks.begin(), m_callbacks.end(),
                                       [&](auto &cb)
                                       {
                                         if(auto locked = cb.lock())
                                         {
                                           locked->m_cb(args...);
                                           return false;
                                         }
                                         return true;
                                       });
          m_callbacks.erase(newEnd, m_callbacks.end());
        }
      }

      Connection connectWithInit(const Callback &cb)
      {
        auto ptr = std::make_shared<Detail::Connection<Args...>>(cb);
        m_callbacks.push_back(ptr);
        std::apply([&](const auto &...a) { cb(a...); }, m_cache);
        return ptr;
      }

      Connection connectWithoutInit(const Callback &cb)
      {
        auto ptr = std::make_shared<Detail::Connection<Args...>>(cb);
        m_callbacks.push_back(ptr);
        return ptr;
      }

      const std::tuple<Args...> &getCache() const
      {
        return m_cache;
      }

     private:
      std::tuple<Args...> m_cache;
      std::vector<std::weak_ptr<Detail::Connection<Args...>>> m_callbacks;
    };
  }
}