#pragma once

#include <ui/Types.h>
#include "tools/ReactiveVar.h"
#include "ui/touch-ui/Interface.h"

namespace Ui::Touch
{
  template <Section s, typename Base> class SectionWrapper : public Base
  {
   public:
    explicit SectionWrapper(Touch::Interface &touch)
    {
      this->get_style_context()->add_class("section");
      
      m_computations.add(
          [this, &touch]
          {
            if(touch.getCurrentSection() == Section::Toolboxes)
              this->get_style_context()->add_class("selected");
            else
              this->get_style_context()->remove_class("selected");
          });
    }

   protected:
    Tools::DeferredComputations m_computations;
  };
}