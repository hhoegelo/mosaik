#include "Mixer.h"

namespace Ui::Touch
{
  Mixer::Mixer(Touch::Interface& touch)
      : SectionWrapper(touch)
  {
    get_style_context()->add_class("mixer");
  }
}