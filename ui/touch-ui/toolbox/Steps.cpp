#include "Steps.h"
#include "core/Types.h"
#include "SoftButtonGrid.h"
#include "KnobGrid.h"
#include <core/api/Interface.h>
#include <gtkmm/label.h>
#include <gtkmm/grid.h>

namespace Ui::Touch
{

  Steps::Steps(Core::Api::Interface &core)
      : Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL)
      , m_core(core)
      , m_computations(Glib::MainContext::get_default())
  {
    get_style_context()->add_class("steps-wizard");

    auto knobs = Gtk::manage(new KnobGrid());
    auto buttons = Gtk::manage(new SoftButtonGrid(SoftButtonGrid::Where::Right));

    auto addKnob = [&](const char *title, Core::ParameterId id, Knob k)
    { knobs->set(k, title, [this, id] { return m_core.getFirstSelectedTileParameterDisplay(id); }); };

    auto addButton = [buttons](const char *title, SoftButton s) { buttons->set(s, title, [] { return ""; }); };

    auto addRadioButton = [&](const char *title, Core::WizardMode expected, SoftButton s)
    {
      buttons->set(s, title,
                   [this, expected]
                   {
                     auto t = m_core.getSelectedTiles().front();
                     return std::get<uint8_t>(m_core.getParameter(t, Core::ParameterId::WizardMode)) == expected
                         ? "On"
                         : "Off";
                   });
    };

    addKnob("Rotate", Core::ParameterId::WizardRotate, Knob::Rightmost);
    addKnob("Ons", Core::ParameterId::WizardOns, Knob::NorthEast);
    addKnob("Offs", Core::ParameterId::WizardOffs, Knob::Center);

    addRadioButton("Or", Core::WizardMode::Or, SoftButton::Right_SouthWest);
    addRadioButton("And", Core::WizardMode::And, SoftButton::Right_South);
    addRadioButton("Replace", Core::WizardMode::Replace, SoftButton::Right_SouthEast);
    addRadioButton("Not", Core::WizardMode::Not, SoftButton::Right_East);

    addButton("Reset", SoftButton::Right_West);
    addButton("Apply", SoftButton::Right_Center);

    pack_start(*knobs);
    pack_start(*buttons);
  }
}