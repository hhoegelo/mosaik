#pragma once

#include <gtkmm/box.h>
#include "tools/ReactiveVar.h"

namespace Core::Api
{
  class Interface;
}

namespace Ui::Touch
{
  class Steps : public Gtk::Box
  {
   public:
    explicit Steps(Core::Api::Interface &core);

   private:
    Core::Api::Interface &m_core;
    Tools::DeferredComputations m_computations;
  };
}