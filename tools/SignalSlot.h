#pragma once

#include <memory>
#include <functional>
#include <algorithm>

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

      template <typename Signature> class Connection : public ConnectionBase
      {
       public:
        using Callback = std::function<Signature>;
        Connection(const Callback &cb)
            : m_cb(cb)
        {
        }

        Callback m_cb;
      };
    }

    using Connection = std::shared_ptr<Detail::ConnectionBase>;

    template <typename Signature> class Signal
    {
     public:
      using Callback = std::function<Signature>;

     public:
      template <typename... Args> void emit(Args &&...args)
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

      Connection connect(const Callback &cb)
      {
        auto ptr = std::make_shared<Detail::Connection<Signature>>(cb);
        m_callbacks.push_back(ptr);
        return ptr;
      }

     private:
      std::vector<std::weak_ptr<Detail::Connection<Signature>>> m_callbacks;
    };
  }
}