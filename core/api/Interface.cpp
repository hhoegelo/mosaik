#include "Interface.h"

namespace Core
{
  namespace Api
  {
    Tools::Signals::Connection Interface::connect(ChannelId channelId, ParameterId parameterId,
                                                  const std::function<void(const ParameterValue &)> &cb)
    {
      auto &c = m_parameterCache[std::make_tuple(channelId, parameterId)];
      cb(c.cache);
      return c.sig.connect(cb);
    }

    void Interface::commit(ChannelId channelId, ParameterId parameterId, const ParameterValue &v)
    {
      auto &c = m_parameterCache[std::make_tuple(channelId, parameterId)];
      c.cache = v;
      c.sig.emit(v);
    }
  }
}