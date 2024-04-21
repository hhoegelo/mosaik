#include "Main.h"

namespace Ui::Touch
{
  Main::Main(Touch::Interface& touch)
      : SectionWrapper(touch)
  {
    get_style_context()->add_class("main");
  }
}