#pragma once

#include <core/Types.h>
#include <gtkmm/grid.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  class Pattern : public Gtk::Grid
  {
   public:
    Pattern(Core::Api::Interface &core);

   private:
    void reconnectPattern();
    void rebuildPattern();

    Core::Api::Interface &m_core;
    std::vector<sigc::connection> m_patternConnections;
    std::vector<sigc::connection> m_clickConnections;

    Core::Pattern m_mergedPattern;
  };
}
