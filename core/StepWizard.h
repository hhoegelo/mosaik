#pragma once

#include <core/Types.h>

namespace Core
{
  [[nodiscard]] Pattern processWizard(const Pattern &in, Core::WizardMode mode, int8_t rotate, uint8_t ons,
                                      uint8_t offs);

}
