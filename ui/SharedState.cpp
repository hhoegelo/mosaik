#include "SharedState.h"

namespace Ui
{
  SharedState::SharedState()
  {
    m_selection.emit(Toolboxes::Global);
  }

  void SharedState::select(SharedState::Toolboxes t)
  {
    m_selection.emit(t);
  }

  Tools::Signals::Connection SharedState::connectSelectedToolbox(const std::function<void(Toolboxes)> &cb)
  {
    return m_selection.connectWithInit(cb);
  }

}