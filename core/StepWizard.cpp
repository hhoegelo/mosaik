#include "StepWizard.h"
#include <core/DataModel.h>
#include <array>

namespace Core
{
  static Pattern generate(int8_t rotate, uint8_t ons, uint8_t offs)
  {
    Pattern ret = {};

    if(!ons && !offs)
      return ret;

    for(int i = 0; i < NUM_STEPS;)
    {
      for(int x = 0; x < ons; x++)
      {
        if(i < NUM_STEPS)
          ret[i] = true;

        i++;
      }

      i += offs;
    }

    if(rotate > 0)
      std::rotate(ret.rbegin(), ret.rbegin() + std::abs(rotate), ret.rend());
    else if(rotate < 0)
      std::rotate(ret.begin(), ret.begin() + std::abs(rotate), ret.end());

    return ret;
  }

  [[nodiscard]] Pattern processWizard(const Pattern &in, Core::WizardMode mode, int8_t rotate, uint8_t ons,
                                      uint8_t offs)
  {
    Pattern lhs = in;
    Pattern rhs = generate(rotate, ons, offs);

    for(int i = 0; i < NUM_STEPS; i++)
    {
      switch(mode)
      {
        case Or:
          lhs[i] |= rhs[i];
          break;

        case And:
          lhs[i] &= rhs[i];
          break;

        case Replace:
          lhs[i] = rhs[i];
          break;

        case Not:
          if(rhs[i])
            lhs[i] = false;
          break;
      }
    }

    return lhs;
  }

}