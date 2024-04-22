#pragma once

#include "core/api/Interface.h"
#include <gtkmm/drawingarea.h>
#include <gtkmm/styleproperty.h>

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  class WaveformThumb : public Gtk::DrawingArea
  {
   public:
    WaveformThumb(Core::Api::Interface& core, Core::TileId tileId);

   private:
    Gtk::StyleProperty<guint> m_foo;
  };
}