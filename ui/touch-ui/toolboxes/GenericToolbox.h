#pragma once

#include "Toolbox.h"
#include "KnobGrid.h"
#include "SoftButtonGrid.h"
#include "ui/ParameterDescriptor.h"
#include "ui/ToolboxDefinition.h"
#include "ui/Controller.h"
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
      auto headline = Gtk::manage(new Gtk::Label(ToolboxDefinition<T>::title));
      auto headlineBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
      headlineBox->get_style_context()->add_class("header");
      headline->set_halign(Gtk::Align::ALIGN_FILL);
      headline->set_justify(Gtk::JUSTIFY_LEFT);
      headlineBox->add(*headline);
      pack_start(*headlineBox);

      bool hasKnobs = false;
      bool hasButtons = false;

      Ui::ToolboxDefinition<T>::MaximizedParameters::forEach(
          [&](auto a)
          {
            hasKnobs |= std::holds_alternative<Knob>(decltype(a)::position);
            hasButtons |= std::holds_alternative<SoftButton>(decltype(a)::position);
          });

      Ui::ToolboxDefinition<T>::MaximizedCustom::forEach(
          [&](auto a)
          {
            hasKnobs |= std::holds_alternative<Knob>(decltype(a)::position);
            hasButtons |= std::holds_alternative<SoftButton>(decltype(a)::position);
          });

      if(hasKnobs)
      {
        auto knobs = Gtk::manage(new KnobGrid());
        Ui::ToolboxDefinition<T>::MaximizedParameters::forEach(
            [&](auto a)
            {
              using B = decltype(a);
              if(std::holds_alternative<Knob>(B::position))
                knobs->set(std::get<Knob>(B::position), ParameterDescriptor<B::id>::title, B::color,
                           [&] { return controller.getDisplayValue(B::id); });
            });

        Ui::ToolboxDefinition<T>::MaximizedCustom::forEach(
            [&](auto a)
            {
              using B = decltype(a);
              if(std::holds_alternative<Knob>(B::position))
                knobs->set(std::get<Knob>(B::position), B::ID::title, B::color,
                           [&] { return controller.getDisplayValue(typename B::ID {}); });
            });

        pack_start(*knobs);
      }

      if(hasButtons)
      {
        bool addedLButton = false;
        bool addedRButton = false;
        auto lButtons = Gtk::manage(new SoftButtonGrid(SoftButtonGrid::Where::Left));
        auto rButtons = Gtk::manage(new SoftButtonGrid(SoftButtonGrid::Where::Right));

        Ui::ToolboxDefinition<T>::MaximizedParameters::forEach(
            [&](auto a)
            {
              using B = decltype(a);
              if(std::holds_alternative<SoftButton>(B::position))
              {
                addedLButton |= lButtons->set(std::get<SoftButton>(B::position), ParameterDescriptor<B::id>::title,
                                              B::color, [&] { return controller.getDisplayValue(B::id); });
                addedRButton |= rButtons->set(std::get<SoftButton>(B::position), ParameterDescriptor<B::id>::title,
                                              B::color, [&] { return controller.getDisplayValue(B::id); });
              }
            });

        Ui::ToolboxDefinition<T>::MaximizedCustom::forEach(
            [&](auto a)
            {
              using B = decltype(a);
              if(std::holds_alternative<SoftButton>(B::position))
              {
                if constexpr(  std::is_same_v<typename B::ID, PreviousToolbox>
                            || std::is_same_v<typename B::ID, NextToolbox>
                            || std::is_same_v<typename B::ID, GotoToolboxTile>
                            || std::is_same_v<typename B::ID, GotoToolboxWaveform>
                            || std::is_same_v<typename B::ID, GotoToolboxGlobal>
                            || std::is_same_v<typename B::ID, GotoToolboxMute>
                            || std::is_same_v<typename B::ID, GotoToolboxSteps>
                            || std::is_same_v<typename B::ID, GotoToolboxSnapshots> )
                  return;

                addedLButton |= lButtons->set(std::get<SoftButton>(B::position), B::ID::title, B::color,
                                              [&] { return controller.getDisplayValue(typename B::ID {}); });
                addedRButton |= rButtons->set(std::get<SoftButton>(B::position), B::ID::title, B::color,
                                              [&] { return controller.getDisplayValue(typename B::ID {}); });
              }
            });

        auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
        box->set_homogeneous(true);

        if(addedLButton || addedRButton)
        {
          box->pack_start(*lButtons);
          box->pack_start(*rButtons);
          pack_start(*box);
        }
      }
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
