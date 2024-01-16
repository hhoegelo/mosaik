#include "StepButton.h"
#include <core/api/Interface.h>

namespace Ui::Touch
{
  StepButton::StepButton(Core::Api::Interface& core, int tileId, int step)
      : Gtk::Button()
  {
    auto connection = signal_clicked().connect(
        [&core, tileId, step, this]
        {
          auto pattern = m_currentPattern;
          pattern[step] = !pattern[step];
          core.setParameter(tileId, Core::ParameterId::Pattern, pattern);
        });

    m_connection = core.connect(tileId, Core::ParameterId::Pattern,
                                [this, step, connection](const Core::ParameterValue& p) mutable
                                {
                                  connection.block();
                                  m_currentPattern = std::get<Core::Pattern>(p);
                                  this->set_label(m_currentPattern[step] ? "x" : " ");
                                  connection.unblock();
                                });
  }
}