#pragma once

#include <gtkmm/drawingarea.h>
#include "tools/ReactiveVar.h"

namespace Ui::Touch
{
  class LevelMeter : public Gtk::DrawingArea
  {
   public:
    LevelMeter(const std::string &classes, const std::function<float()> &cb, float decay = 0.0f);

    void on_size_allocate(Gtk::Allocation &allocation) override;

   private:
    float m_currentValue = 0.0f;
    Tools::DeferredComputations m_computations;
  };
}