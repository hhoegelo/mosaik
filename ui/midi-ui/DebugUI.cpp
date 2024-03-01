#include "DebugUI.h"
#include "Controller.h"

#include <gtkmm/grid.h>

namespace Ui::Midi
{
  auto css = R"(
    .color-red {
      color: red;
    }

    .color-blue {
      color: blue;
    }

    .color-green {
      color: green;
    }

    .color-white {
      color: white;
    }

  )";

  DebugUI::DebugUI(SharedState& sharedUiState, Core::Api::Interface& core)
      : m_core(core)
      , m_ctrl(std::make_unique<Controller>(sharedUiState, core, *this))
  {
  }

  DebugUI::~DebugUI() = default;

  void DebugUI::build()
  {
    auto cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_data(css);
    Glib::RefPtr<Gtk::StyleContext> styleContext = Gtk::StyleContext::create();
    styleContext->add_provider_for_screen(Gdk::Screen::get_default(), cssProvider,
                                          GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    set_title("Mosaik Debug UI");
    set_border_width(10);
    auto screen = Gtk::manage(new Gtk::Grid());

    for(int i = 0; i < 16; i++)
    {
      screen->attach(*buildStep(i), i, 0);
      screen->attach(*buildStep(i + 16), 16, i);
      screen->attach(*buildStep(i + 32), 16 - i, 16);
      screen->attach(*buildStep(i + 48), 0, 16 - i);
    }

    screen->attach(*buildButton(SoftButton::Left_NorthWest), 2, 10);
    screen->attach(*buildButton(SoftButton::Left_North), 3, 10);
    screen->attach(*buildButton(SoftButton::Left_West), 2, 12);
    screen->attach(*buildButton(SoftButton::Left_Center), 3, 12);
    screen->attach(*buildButton(SoftButton::Left_East), 4, 12);
    screen->attach(*buildButton(SoftButton::Left_SouthWest), 2, 14);
    screen->attach(*buildButton(SoftButton::Left_South), 3, 14);
    screen->attach(*buildButton(SoftButton::Left_SouthEast), 4, 14);

    screen->attach(*buildButton(SoftButton::Right_North), 13, 10);
    screen->attach(*buildButton(SoftButton::Right_NorthEast), 14, 10);
    screen->attach(*buildButton(SoftButton::Right_West), 12, 12);
    screen->attach(*buildButton(SoftButton::Right_Center), 13, 12);
    screen->attach(*buildButton(SoftButton::Right_East), 14, 12);
    screen->attach(*buildButton(SoftButton::Right_SouthWest), 12, 14);
    screen->attach(*buildButton(SoftButton::Right_West), 13, 14);
    screen->attach(*buildButton(SoftButton::Right_SouthEast), 14, 14);

    screen->attach(*buildKnob(Knob::Leftmost), 5, 9);
    screen->attach(*buildKnob(Knob::Rightmost), 11, 9);
    screen->attach(*buildKnob(Knob::NorthWest), 7, 10);
    screen->attach(*buildKnob(Knob::NorthEast), 9, 10);
    screen->attach(*buildKnob(Knob::Center), 8, 12);
    screen->attach(*buildKnob(Knob::SouthWest), 7, 14);
    screen->attach(*buildKnob(Knob::SouthEast), 9, 14);

    add(*screen);
    show_all();

    m_ctrl->kickOff();
  }

  Gtk::Widget* DebugUI::buildStep(Step i)
  {
    auto btn = Gtk::manage(new Gtk::Button(std::to_string(1 + i)));
    btn->set_name("step-" + std::to_string(i));

    btn->signal_clicked().connect(
        [this, btn, i]
        {
          auto merged = m_core.getMergedPattern(nullptr);
          auto state = merged[i];
          m_core.setStep(i, !state);
        });

    return btn;
  }

  Gtk::Widget* DebugUI::buildKnob(Knob knob)
  {
    auto w = Gtk::manage(new Gtk::Button("K"));
    w->set_name(getKnobName(knob));
    return w;
  }

  Gtk::Widget* DebugUI::buildButton(SoftButton button)
  {
    auto btn = Gtk::manage(new Gtk::Button("B"));
    btn->set_name(getSoftButtonName(button));
    return btn;
  }

  void DebugUI::setSoftButtonColor(SoftButton button, Color c)
  {
    auto name = getSoftButtonName(button);
    setColor(name, c);
  }

  void DebugUI::setStepButtonColor(Step step, Color c)
  {
    auto name = "step-" + std::to_string(step);
    setColor(name, c);
  }

  void DebugUI::onSoftButtonEvent(std::function<void(SoftButton, ButtonEvent)> cb)
  {
    m_onSoftButtonEvent = cb;
  }

  void DebugUI::onStepButtonEvent(std::function<void(Step, ButtonEvent)> cb)
  {
    m_onStepButtonEvent = cb;
  }

  void DebugUI::onKnobEvent(std::function<void(Knob, int)> cb)
  {
    m_onKnobEvent = cb;
  }

  void DebugUI::setColor(const std::string& widgetName, Color c)
  {
    auto cssClass = "color-" + getColorName(c);
    auto grid = dynamic_cast<Gtk::Grid*>(get_child());
    const auto& children = grid->get_children();
    auto widgetIt
        = std::find_if(children.begin(), children.end(), [&](Gtk::Widget* c) { return c->get_name() == widgetName; });

    if(widgetIt != children.end())
    {
      auto widget = dynamic_cast<Gtk::Widget*>(*widgetIt);
      auto style = widget->get_style_context();
      for(const auto& color : style->list_classes())
      {
        if(color.find("color-") == 0)
          if(color != cssClass)
            style->remove_class(color);
      }
      style->add_class(cssClass);
    }
  }
}
