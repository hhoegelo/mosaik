#include "Pattern.h"

#include <core/Types.h>
#include <core/api/Interface.h>

#include <gtkmm/checkbutton.h>

#include <iostream>

Ui::Touch::Pattern::Pattern(Core::Api::Interface& core)
    : m_core(core)
{
  get_style_context()->add_class("pattern");

  // connect to selected signal of all tiles
  for(int i = 0; i < NUM_TILES; i++)
  {
    core.connect(i, Core::ParameterId::Selected, [this](const Core::ParameterValue& p) mutable { reconnectPattern(); });
  }

  for(int row = 0; row < 4; row++)
  {
    for(int col = 0; col < 16; col++)
    {
      int step = row * 16 + col;

      auto check = Gtk::manage(new Gtk::CheckButton());
      check->get_style_context()->add_class("step");

      m_clickConnections.push_back(check->signal_clicked().connect(
          [&core, step, this]
          {
            auto pattern = m_mergedPattern;
            pattern[step] = !pattern[step];

            std::cout << __FUNCTION__ << ": " << __LINE__ << std::endl;

            for(auto tileId : core.getSelectedTiles())
              core.setParameter(tileId, Core::ParameterId::Pattern, pattern);

            std::cout << __FUNCTION__ << ": " << __LINE__ << std::endl;
          }));

      attach(*check, col, row, 1, 1);
    }
  }
}
void Ui::Touch::Pattern::reconnectPattern()
{
  std::cout << __FUNCTION__ << ": " << __LINE__ << std::endl;

  for(auto& c : m_patternConnections)
    c.disconnect();

  m_patternConnections.clear();

  for(int i = 0; i < NUM_TILES; i++)
  {
    if(get<bool>(m_core.getParameter(i, Core::ParameterId::Selected)))
    {
      m_patternConnections.push_back(m_core.connect(
          i, Core::ParameterId::Pattern, [this](const Core::ParameterValue& p) mutable { rebuildPattern(); }));
    }
  }
  std::cout << __FUNCTION__ << ": " << __LINE__ << std::endl;
}

void Ui::Touch::Pattern::rebuildPattern()
{
  std::cout << __FUNCTION__ << ": " << __LINE__ << std::endl;

  m_mergedPattern = {};

  for(int i = 0; i < NUM_TILES; i++)
  {
    if(get<bool>(m_core.getParameter(i, Core::ParameterId::Selected)))
    {
      auto tilePattern = get<Core::Pattern>(m_core.getParameter(i, Core::ParameterId::Pattern));

      for(size_t s = 0; s < tilePattern.size(); s++)
        m_mergedPattern[s] |= tilePattern[s];
    }
  }

  for(auto& c : m_patternConnections)
    c.block();

  for(auto& c : m_clickConnections)
    c.block();

  for(int row = 0; row < 4; row++)
  {
    for(int col = 0; col < 16; col++)
    {
      int step = row * 16 + col;
      if(auto checkbox = dynamic_cast<Gtk::CheckButton*>(get_child_at(col, row)))
        checkbox->set_active(m_mergedPattern[step]);
    }
  }

  for(auto& c : m_clickConnections)
    c.unblock();

  for(auto& c : m_patternConnections)
    c.unblock();

  std::cout << __FUNCTION__ << ": " << __LINE__ << std::endl;
}