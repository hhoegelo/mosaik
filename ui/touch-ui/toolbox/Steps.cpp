#include "Steps.h"
#include "core/Types.h"
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
    auto knobs = Gtk::manage(new Gtk::Grid());
    auto buttons = Gtk::manage(new Gtk::Grid());

    auto addKnob = [&](const char *title, Core::ParameterId id, int x, int y)
    {
      auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
      box->add(*Gtk::manage(new Gtk::Label(title)));
      auto level = Gtk::manage(new Gtk::Label());
      box->add(*level);
      knobs->attach(*box, x, y, 1, 1);

      m_computations.add([this, level, id] { level->set_label(m_core.getFirstSelectedTileParameterDisplay(id)); });
    };

    auto addButton = [&](const char *title, int x, int y)
    {
      auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
      box->add(*Gtk::manage(new Gtk::Label(title)));
      auto level = Gtk::manage(new Gtk::Label());
      box->add(*level);
      buttons->attach(*box, x, y, 1, 1);
    };

    auto addRadioButton = [&](const char *title, Core::WizardMode expected, int x, int y)
    {
      auto box = Gtk::manage(new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL));
      box->add(*Gtk::manage(new Gtk::Label(title)));
      auto level = Gtk::manage(new Gtk::Label());
      box->add(*level);
      buttons->attach(*box, x, y, 1, 1);
      m_computations.add(
          [this, level, expected]
          {
            auto t = m_core.getSelectedTiles().front();
            level->set_label(
                std::get<uint8_t>(m_core.getParameter(t, Core::ParameterId::WizardMode)) == expected ? "On" : "Off");
          });
    };

    addKnob("Rotate", Core::ParameterId::WizardRotate, 4, 0);
    addKnob("Ons", Core::ParameterId::WizardOns, 3, 1);
    addKnob("Offs", Core::ParameterId::WizardOffs, 2, 2);

    addRadioButton("Or", Core::WizardMode::Or, 0, 2);
    addRadioButton("And", Core::WizardMode::And, 1, 2);
    addRadioButton("Replace", Core::WizardMode::Replace, 2, 2);
    addRadioButton("Not", Core::WizardMode::Not, 2, 1);
    addButton("Reset", 0, 1);
    addButton("Apply", 1, 1);

    pack_start(*knobs);
    pack_start(*buttons);
  }
}