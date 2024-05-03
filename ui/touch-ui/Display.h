#pragma once

#include <core/Types.h>
#include <core/api/Interface.h>
#include <string>

namespace Ui::Touch
{
  std::string FFFgetDisplayValue(Core::Api::Interface &core, Core::TileId tile, Core::ParameterId id);
  std::string FFFgetDisplayValue(Core::Api::Interface &core, Core::ParameterId id);

}