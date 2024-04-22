#pragma once

#include "tools/ReactiveVar.h"

#include "core/Types.h"
#include "core/api/Interface.h"
#include "WaveformThumb.h"

#include <gtkmm/grid.h>
#include <gtkmm/levelbar.h>
#include <gtkmm/styleproperty.h>

namespace Core::Api
{
  class Interface;
}

namespace Dsp::Api::Display
{
  class Interface;
}

namespace Ui::Touch
{
  class Tile : public Gtk::Grid
  {
   public:
    Tile(Core::Api::Interface &core, Dsp::Api::Display::Interface &dsp, Core::TileId tileId);

    void on_size_allocate(Gtk::Allocation &allocation);

   protected:
    Gtk::SizeRequestMode get_request_mode_vfunc() const override;
    void get_preferred_height_vfunc(int &minimum_height, int &natural_height) const override;
    void get_preferred_width_vfunc(int &minimum_width, int &natural_width) const override;

   private:
    void runLevelMeterTimer(Dsp::Api::Display::Interface &dsp, Core::TileId tileId, Gtk::DrawingArea *l,
                            Gtk::DrawingArea *r);

    Gtk::Label *addSampleName();
    Ui::Touch::WaveformThumb *addWaveform(Core::Api::Interface &core, const Core::TileId &tileId);
    Gtk::Label *addDurationLabel();
    std::array<Gtk::Label *, 64> addSteps();
    std::string formatTime(long ms) const;

    Gtk::StyleProperty<int> m_size;
    Tools::DeferredComputations m_computations;
    std::tuple<float, float> m_levels {};
  };
}