#include "DebugUI.h"
#include "ui/Controller.h"
#include "Erp.h"
#include <tools/Format.h>
#include <gtkmm/grid.h>

namespace Ui::Midi
{
  auto css = R"(
    .color-red {
      background: red;
    }

    .color-blue {
      background: blue;
    }

    .color-green {
      background: green;
    }

    .color-white {
      background: white;
    }

    .color-purple {
      background: purple;
    }

    button.current-step {
      background: orange;
    }

  )";

  DebugUI::DebugUI(Core::Api::Interface& core, Dsp::Api::Display::Interface& dsp, ::Ui::Touch::Interface& touchUi,
                   Ui::Controller& controller)
      : m_core(core)
      , m_ctrl(controller)
  {
    build();
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
    screen->attach(*buildButton(SoftButton::Right_South), 13, 14);
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
  }

  Gtk::Widget* DebugUI::buildStep(Step i)
  {
    auto btn = Gtk::manage(new Gtk::Button(std::to_string(1 + i)));
    btn->set_name("step-" + std::to_string(i));

    btn->signal_clicked().connect(
        [this, i]
        {
          m_ctrl.onStepButtonEvent(i, ButtonEvent::Press);
          m_ctrl.onStepButtonEvent(i, ButtonEvent::Release);
        });

    return btn;
  }

  Gtk::Widget* DebugUI::buildKnob(Knob knob)
  {
    auto w = Gtk::manage(new Erp());
    w->set_name(Tools::format("Knob-%d", static_cast<int>(knob)));
    w->connect([this, knob](auto inc) { m_ctrl.onErpInc(knob, inc); });
    w->down([this, knob]() { m_ctrl.onSoftButtonEvent(getButtonForKnob(knob), ButtonEvent::Press); });
    w->up([this, knob]() { m_ctrl.onSoftButtonEvent(getButtonForKnob(knob), ButtonEvent::Release); });
    auto r = Gtk::manage(new Gtk::EventBox());
    r->add(*w);
    r->add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON_PRESS_MASK);
    return r;
  }

  Gtk::Widget* DebugUI::buildButton(SoftButton button)
  {
    auto btn = Gtk::manage(new Gtk::Button("B"));
    btn->set_name(Tools::format("SoftButton-%d", static_cast<int>(button)));
    btn->signal_pressed().connect([this, button] { m_ctrl.onSoftButtonEvent(button, ButtonEvent::Press); });
    btn->signal_released().connect([this, button] { m_ctrl.onSoftButtonEvent(button, ButtonEvent::Release); });
    return btn;
  }

  static Gtk::Widget* findChildWidget(const Gtk::Widget* p, const char* name)
  {
    auto pThis = const_cast<Gtk::Widget*>(p);

    if(auto b = reinterpret_cast<const Gtk::Buildable*>(p))
    {
      if(b->get_name() == name)
        return pThis;
    }

    if(pThis->get_name() == name)
      return pThis;

    if(auto c = dynamic_cast<const Gtk::Container*>(pThis))
      for(auto child : c->get_children())
        if(auto found = findChildWidget(child, name))
          return found;

    return nullptr;
  }

  const Gtk::Widget* DebugUI::findChild(const std::string& name)
  {
    return findChildWidget(this, name.c_str());
  }

  void DebugUI::setColor(const std::string& widgetName, Color c)
  {
    if(auto widget = findChild(widgetName))
    {
      setColor(widget, c);
    }
  }

  void DebugUI::setColor(const Gtk::Widget* widget, Color c)
  {
    auto cssClass = "color-" + getColorName(c);

    auto style = widget->get_style_context();
    for(const auto& color : style->list_classes())
    {
      if(color.find("color-") == 0)
        if(color != cssClass)
          style->remove_class(color);
    }
    style->add_class(cssClass);
  }

  void DebugUI::setColor(const Gtk::Widget* widget, uint8_t r, uint8_t g, uint8_t b)
  {
    auto ctx = widget->get_style_context();
    auto id = std::to_string(reinterpret_cast<uint64_t>(widget));
    ctx->add_class(id);

    auto style = widget->get_style_context();
    for(const auto& color : style->list_classes())
    {
      if(color.find("color-") == 0)
        style->remove_class(color);
    }

    GdkRGBA color;
    color.red = r / 127.0;
    color.green = g / 127.0;
    color.blue = b / 127.0;
    color.alpha = 0.0f;

    static std::map<const Gtk::Widget*, Glib::RefPtr<Gtk::CssProvider>> s_providers;

    auto css = Tools::format(R"(
.%s {
  background-image: none;
  background-color: rgb(%d, %d, %d);
}
)",
                             id.c_str(), 2 * r, 2 * g, 2 * b);

    if(!s_providers.count(widget))
    {
      auto p = Gtk::CssProvider::create();
      s_providers[widget] = p;
      ctx->add_provider_for_screen(Gdk::Screen::get_default(), p, GTK_STYLE_PROVIDER_PRIORITY_USER);
      p->load_from_data(css);
    }
    else
    {
      s_providers.at(widget)->load_from_data(css);
    }
  }

  void DebugUI::setLed(Knob l, Color c)
  {
    if(auto k = findChild(Tools::format("Knob-%d", static_cast<int>(l))))
    {
      setColor(k, c);
    }
  }

  void DebugUI::setLed(SoftButton l, Color c)
  {
    if(auto sb = findChild(Tools::format("SoftButton-%d", static_cast<int>(l))))
    {
      setColor(sb, c);
    }
  }

  void DebugUI::setLed(Step l, Color c)
  {
    if(l <= static_cast<Step>(Led::Step_63))
    {
      setColor("step-" + std::to_string(static_cast<int>(l)), c);
    }
  }

  void DebugUI::setLed(SoftButton s, uint8_t r, uint8_t g, uint8_t b)
  {
    if(auto sb = findChild(Tools::format("SoftButton-%d", static_cast<int>(s))))
    {
      setColor(sb, r, g, b);
    }
  }
}
