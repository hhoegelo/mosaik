#include "Interface.h"

namespace Core
{
  namespace Api
  {
    Tools::Signals::Connection Interface::connect(Col col, Row row, ParameterId id,
                                                  const std::function<void(ParameterValue)> &cb)
    {
      auto &c = m_perChannelParameterCache[std::make_tuple(col, row, id)];
      cb(c.cache);
      return c.sig.connect(cb);
    }

    Tools::Signals::Connection Interface::connect(Col col, Row row, const std::function<void(const Path &)> &cb)
    {
      auto &c = m_perChannelPathCache[std::make_tuple(col, row)];
      cb(c.cache);
      return c.sig.connect(cb);
    }

    Tools::Signals::Connection Interface::connect(ParameterId id, const std::function<void(ParameterValue)> &cb)
    {
      auto &c = m_globalParameterCache[id];
      cb(c.cache);
      return c.sig.connect(cb);
    }

    void Interface::commit(Col col, Row row, ParameterId id, ParameterValue v)
    {
      auto &c = m_perChannelParameterCache[std::make_tuple(col, row, id)];
      c.cache = v;
      c.sig.emit(v);
    }

    void Interface::commit(Col col, Row row, const Path &v)
    {
      auto &c = m_perChannelPathCache[std::make_tuple(col, row)];
      c.cache = v;
      c.sig.emit(v);
    }

    void Interface::commit(ParameterId id, ParameterValue v)
    {
      auto &c = m_globalParameterCache[id];
      c.cache = v;
      c.sig.emit(v);
    }
  }
}