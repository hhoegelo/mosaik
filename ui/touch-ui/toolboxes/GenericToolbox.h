#pragma once

#include "Toolbox.h"
#include "SoftButtonGrid.h"
#include "ui/ParameterDescriptor.h"
#include "ui/ToolboxDefinition.h"
#include "ui/Controller.h"
#include "CombinedGrid.h"
#include <gtkmm/label.h>

namespace Ui
{
  class Controller;
}

namespace Ui::Touch
{
  template <Ui::Toolbox T> class GenericMaximized : public Gtk::Box
  {
   public:
    GenericMaximized(Ui::Controller &controller)
        : Gtk::Box(Gtk::ORIENTATION_VERTICAL)
    {
      get_style_context()->add_class("maximized");
      //auto headline = Gtk::manage(new Gtk::Label(ToolboxDefinition<T>::title));
      //auto headlineBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
      //headlineBox->get_style_context()->add_class("header");
      //headline->set_halign(Gtk::Align::ALIGN_FILL);
      //headline->set_justify(Gtk::JUSTIFY_LEFT);
      //headlineBox->add(*headline);
      //pack_start(*headlineBox);

      auto grid = Gtk::manage(new CombinedGrid(ToolboxDefinition<T>::title));

      Ui::ToolboxDefinition<T>::Entires::forEach(
          [&](auto a)
          {
            using B = decltype(a);
            if(std::holds_alternative<Knob>(B::position))
              grid->set(std::get<Knob>(B::position), B::ID::title, B::color,
                        [&]
                        {
                          if constexpr(requires() { controller.getDisplayValue(B::ID::id); })
                            return controller.getDisplayValue(B::ID::id);

                          return controller.getDisplayValue(typename B::ID {});
                        });
          });

      Ui::ToolboxDefinition<T>::Entires::forEach(
          [&](auto a)
          {
            using B = decltype(a);
            if(std::holds_alternative<SoftButton>(B::position))
            {
              if constexpr(std::is_same_v<typename B::ID, PreviousToolbox>
                           || std::is_same_v<typename B::ID, NextToolbox>
                           || std::is_same_v<typename B::ID, GotoToolboxTile>
                           || std::is_same_v<typename B::ID, GotoToolboxWaveform>
                           || std::is_same_v<typename B::ID, GotoToolboxGlobal>
                           || std::is_same_v<typename B::ID, GotoToolboxMute>
                           || std::is_same_v<typename B::ID, GotoToolboxSteps>
                           || std::is_same_v<typename B::ID, GotoToolboxReverb>
                           || std::is_same_v<typename B::ID, GotoToolboxSnapshots>)
                return;

              grid->set(std::get<SoftButton>(B::position), B::ID::title, B::color,
                        [&] { return controller.getDisplayValue(typename B::ID {}); });
            }
          });

      pack_start(*grid);
    }
  };

  template <Ui::Toolbox T> class GenericToolbox : public Toolbox
  {

   public:
    GenericToolbox(ToolboxesInterface &toolboxes, Ui::Controller &controller, Gtk::Widget *maximized = nullptr)
        : Toolbox(toolboxes, T, maximized ? maximized : buildMaximized(controller))
    {
      get_style_context()->add_class("generic toolbox");
    }

    static Gtk::Widget *buildMaximized(Ui::Controller &controller)
    {
      return new GenericMaximized<T>(controller);
    }
  };
}
