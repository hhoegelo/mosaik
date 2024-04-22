#include "Tile.h"
#include "core/api/Interface.h"
#include "dsp/api/display/Interface.h"
#include "WaveformThumb.h"
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <glibmm/main.h>
#include <gtkmm/drawingarea.h>

#include <iostream>

namespace Ui::Touch
{
  namespace
  {
    double dB(double level)
    {
      if(level <= 0)
        return -80;
      return 20 * std::log10(level);
    }

    std::string toCss(double db)
    {
      int rounded_dB = std::clamp(static_cast<int>(std::round(db / -10.0) * -10), -80, 0);
      return "level-" + std::to_string(std::abs(rounded_dB)) + "db";
    }
  }

  Tile::Tile(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, Core::TileId tileId)
      : Glib::ObjectBase("Tile")
      , Gtk::Grid()
      , m_size(*this, "size", 50)
  {
    get_style_context()->add_class("tile");
    set_column_homogeneous(true);
    set_row_homogeneous(true);

    std::array<Gtk::Label*, NUM_STEPS> steps;

    auto buildStep = [&steps](int i)
    {
      auto label = Gtk::manage(new Gtk::Label(""));
      label->set_name("step-" + std::to_string(i));
      label->get_style_context()->add_class("step");
      steps[i] = label;
      return label;
    };

    for(int i = 0; i < 16; i++)
    {
      attach(*buildStep(i), i, 0);
      attach(*buildStep(i + 16), 16, i);
      attach(*buildStep(i + 32), 16 - i, 16);
      attach(*buildStep(i + 48), 0, 16 - i);
    }

    auto sampleName = Gtk::manage(new Gtk::Label(""));
    sampleName->get_style_context()->add_class("sample-file");
    sampleName->property_halign() = Gtk::Align::ALIGN_START;
    sampleName->property_valign() = Gtk::Align::ALIGN_CENTER;
    sampleName->set_ellipsize(Pango::EllipsizeMode::ELLIPSIZE_START);
    attach(*sampleName, 1, 1, 15, 2);

    auto waveform = Gtk::manage(new WaveformThumb(core, tileId));
    attach(*waveform, 1, 3, 10, 10);

    auto seconds = Gtk::manage(new Gtk::Label("0.0s"));
    seconds->get_style_context()->add_class("duration");
    seconds->get_style_context()->add_class("seconds");
    seconds->property_halign() = Gtk::Align::ALIGN_START;
    seconds->property_valign() = Gtk::Align::ALIGN_CENTER;
    attach(*seconds, 1, 14, 15, 2);

    m_computations.add([&core, tileId, sampleName]()
                       { sampleName->set_label(core.getParameterDisplay(tileId, Core::ParameterId::SampleFile)); });

    m_computations.add(
        [&core, &dsp, tileId, seconds]()
        {
          auto file = std::get<Core::Path>(core.getParameter(tileId, Core::ParameterId::SampleFile));

          auto ms = dsp.getDuration(file).count();
          auto s = ms / 1000;
          char txt[256];

          if(s > 0)
            ms -= 1000 * s;

          auto m = s / 60;
          if(m > 0)
            s -= 60 * m;

          auto h = m / 60;
          if(h > 0)
            m -= 60 * h;

          if(h > 0)
            sprintf(txt, "%2d:%02d h", h, m);
          else if(m > 0)
            sprintf(txt, "%2d:%02d m", m, s);
          else if(s > 0)
            sprintf(txt, "%2d.%03d s", s, ms);
          else
            sprintf(txt, "%03d ms", ms);

          seconds->set_label(txt);
        });

    m_computations.add(
        [&core, tileId, this]()
        {
          if(std::get<bool>(core.getParameter(tileId, Core::ParameterId::Selected)))
            get_style_context()->add_class("selected");
          else
            get_style_context()->remove_class("selected");
        });

    m_computations.add(
        [&core, tileId, waveform]()
        {
          auto _ = core.getParameter(tileId, Core::ParameterId::SampleFile);
          waveform->queue_draw();
        });

    m_computations.add(
        [&core, tileId, this, steps]()
        {
          auto pattern = std::get<Core::Pattern>(core.getParameter(tileId, Core::ParameterId::Pattern));

          for(size_t i = 0; i < 64; i++)
          {
            if(pattern[i])
              steps[i]->get_style_context()->add_class("selected");
            else
              steps[i]->get_style_context()->remove_class("selected");
          }
        });

    runLevelMeterTimer(dsp, tileId);
  }

  void Tile::on_size_allocate(Gtk::Allocation& allocation)
  {
    auto m = std::min(allocation.get_height(), allocation.get_width());
    allocation.set_height(m);
    allocation.set_width(m);
    Gtk::Grid::on_size_allocate(allocation);
  }

  void Tile::runLevelMeterTimer(Dsp::Api::Display::Interface& dsp, Core::TileId tileId)
  {
    Glib::signal_timeout().connect([&dsp, tileId, oldDB = -80.0]() mutable { return true; }, 16);
  }

  Gtk::SizeRequestMode Tile::get_request_mode_vfunc() const
  {
    return Gtk::SIZE_REQUEST_CONSTANT_SIZE;
  }

  void Tile::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
  {
    int m = m_size.get_value();
    minimum_height = natural_height = m;
  }

  void Tile::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
  {
    int m = m_size.get_value();
    minimum_width = natural_width = m;
  }
}
