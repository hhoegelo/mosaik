#pragma once

#include <tools/SignalSlot.h>

namespace Ui
{
  class SharedState
  {
   public:
    SharedState();

    enum class Toolboxes
    {
      Global,
      Tile
    };

    void select(Toolboxes t);
    [[nodiscard]] Tools::Signals::Connection connectSelectedToolbox(const std::function<void(Toolboxes)> &cb);

   private:
    Tools::Signals::Signal<Toolboxes> m_selection;
  };
}