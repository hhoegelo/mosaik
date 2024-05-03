#include "Display.h"
#include <array>
#include <ui/ParameterDescriptor.h>

namespace Ui::Touch
{
  template <Core::ParameterId id> struct WrapParameterDescription
  {
    using Wrapped = ParameterDescriptor<id>;
  };

  using GlobalParameters = Core::GlobalParameters<WrapParameterDescription>::Wrapped;
  using TileParameters = Core::TileParameters<WrapParameterDescription>::Wrapped;

  template <typename Description>
  bool fillString(std::string &target, Core::Api::Interface &core, Core::TileId tile, Core::ParameterId id)
  {
    if(Description::id == id)
    {
      target = Description::format(std::get<typename Description::Type>(core.getParameter(tile, id)));
      return true;
    }

    return false;
  }

  std::string FFFgetDisplayValue(Core::Api::Interface &core, Core::TileId tile, Core::ParameterId id)
  {
    std::string ret;
    GlobalParameters globalParams {};

    if(!std::apply([&](auto... a) { return (fillString<decltype(a)>(ret, core, {}, id) || ...); }, GlobalParameters {}))
      std::apply([&](auto... a) { return (fillString<decltype(a)>(ret, core, tile, id) || ...); }, TileParameters {});

    return ret;
  }

  std::string FFFgetDisplayValue(Core::Api::Interface &core, Core::ParameterId id)
  {
    return FFFgetDisplayValue(core, core.getSelectedTile(), id);
  }
}