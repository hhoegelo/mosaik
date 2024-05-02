#pragma once

#include "Toolbox.h"
#include "ui/touch-ui/Display.h"
#include "KnobGrid.h"
#include "SoftButtonGrid.h"
#include <ui/ParameterDescriptor.h>
#include <ui/ToolboxDefinition.h>
#include <gtkmm/label.h>

namespace Ui::Touch
{
  template <Core::ParameterId ID> class GenericMinimizedParameter : public Gtk::Box
  {
   public:
    GenericMinimizedParameter(Core::Api::Interface &core)
        : Gtk::Box(Gtk::ORIENTATION_VERTICAL)
    {
      get_style_context()->add_class("parameter");

      auto name = Gtk::manage(new Gtk::Label(ParameterDescriptor<ID>::title));
      name->get_style_context()->add_class("name");
      pack_start(*name);

      auto value = Gtk::manage(new Gtk::Label());
      value->get_style_context()->add_class("value");
      pack_start(*value);

      m_computations.add([&core, value] { value->set_label(getDisplayValue(core, ID)); });
    }

   private:
    Tools::DeferredComputations m_computations;
  };

  template <Ui::Toolbox T> class GenericMinimized : public Gtk::Box
  {
   public:
    GenericMinimized(Core::Api::Interface &core)
        : Gtk::Box(Gtk::ORIENTATION_HORIZONTAL)
    {
      get_style_context()->add_class("minimized");
      auto headline = Gtk::manage(new Gtk::Label(ToolboxDefinition<T>::title));
      headline->get_style_context()->add_class("header");
      headline->set_halign(Gtk::Align::ALIGN_START);
      pack_start(*headline);

      auto minis = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
      pack_start(*minis);
      Ui::ToolboxDefinition<T>::Minimized::forEach(
          [&](auto a)
          {
            minis->set_halign(Gtk::Align::ALIGN_END);
            minis->pack_start(*Gtk::manage(new GenericMinimizedParameter<decltype(a)::id>(core)), false, false);
          });
    }
  };

  template <Ui::Toolbox T> class GenericMaximized : public Gtk::Box
  {
   public:
    GenericMaximized(Core::Api::Interface &core)
        : Gtk::Box(Gtk::ORIENTATION_VERTICAL)
    {
      get_style_context()->add_class("maximized");
      auto headline = Gtk::manage(new Gtk::Label(ToolboxDefinition<T>::title));
      headline->get_style_context()->add_class("header");
      
      pack_start(*headline);

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
                knobs->set(std::get<Knob>(B::position), ParameterDescriptor<B::id>::title,
                           [&] { return getDisplayValue(core, B::id); });
            });

        Ui::ToolboxDefinition<T>::MaximizedCustom::forEach(
            [&](auto a)
            {
              using B = decltype(a);
              if(std::holds_alternative<Knob>(B::position))
                knobs->set(std::get<Knob>(B::position), B::ID::title, [&] { return "---"; });
            });

        pack_start(*knobs);
      }

      if(hasButtons)
      {
        auto lButtons = Gtk::manage(new SoftButtonGrid(SoftButtonGrid::Where::Left));
        auto rButtons = Gtk::manage(new SoftButtonGrid(SoftButtonGrid::Where::Right));

        Ui::ToolboxDefinition<T>::MaximizedParameters::forEach(
            [&](auto a)
            {
              using B = decltype(a);
              if(std::holds_alternative<SoftButton>(B::position))
              {
                lButtons->set(std::get<SoftButton>(B::position), ParameterDescriptor<B::id>::title,
                              [&] { return getDisplayValue(core, B::id); });
                rButtons->set(std::get<SoftButton>(B::position), ParameterDescriptor<B::id>::title,
                              [&] { return getDisplayValue(core, B::id); });
              }
            });

        Ui::ToolboxDefinition<T>::MaximizedCustom::forEach(
            [&](auto a)
            {
              using B = decltype(a);
              if(std::holds_alternative<SoftButton>(B::position))
              {
                lButtons->set(std::get<SoftButton>(B::position), B::ID::title, [&] { return ""; });
                rButtons->set(std::get<SoftButton>(B::position), B::ID::title, [&] { return ""; });
              }
            });

        auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
        box->set_homogeneous(true);
        box->pack_start(*lButtons);
        box->pack_start(*rButtons);
        pack_start(*box);
      }
    }
  };

  template <Ui::Toolbox T> class GenericToolbox : public Toolbox
  {

   public:
    GenericToolbox(ToolboxesInterface &toolboxes, Core::Api::Interface &core, Gtk::Widget *maximized = nullptr)
        : Toolbox(toolboxes, T, buildMinimized(core), maximized ? maximized : buildMaximized(core))
    {
      get_style_context()->add_class("generic toolbox");
    }

    static Gtk::Widget *buildMinimized(Core::Api::Interface &core)
    {
      return new GenericMinimized<T>(core);
    }

    static Gtk::Widget *buildMaximized(Core::Api::Interface &core)
    {
      return new GenericMaximized<T>(core);
    }
  };
}