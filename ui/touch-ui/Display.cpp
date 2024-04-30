#include "Display.h"
#include <ui/Types.h>

namespace Ui::Touch
{
  template <Core::ParameterId id> struct WrapParameterDescription
  {
    using Wrapped = ParameterDescription<id>;
  };

  using GlobalParameters = Core::GlobalParameters<WrapParameterDescription>::Wrapped;
  using TileParameters = Core::TileParameters<WrapParameterDescription>::Wrapped;

  template <typename Description>
  bool fillString(std::string &target, const Core::ParameterValue &v, Core::ParameterId id)
  {
    if(Description::id == id)
    {
      target = Description::format(std::get<typename Description::Type>(v));
      return true;
    }

    return false;
  }

  std::string getDisplayValue(Core::Api::Interface &core, Core::TileId tile, Core::ParameterId id)
  {
    GlobalParameters globalParams {};
    TileParameters tileParams {};
    return std::apply(
        [&](auto... a)
        {
          std::string ret;
          auto value = core.getParameter(tile, id);
          (fillString<decltype(a)>(ret, value, id) || ...);
          return ret;
        },
        std::tuple_cat(globalParams, tileParams));
  }

  std::string getDisplayValue(Core::Api::Interface &core, Core::ParameterId id)
  {
    return getDisplayValue(core, core.getSelectedTile(), id);
  }
}