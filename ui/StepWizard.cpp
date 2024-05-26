#include "StepWizard.h"
#include <vector>
#include <cstdint>
#include <algorithm>
#include <limits>

namespace Ui
{
  std::vector<Core::Pattern> buildPatterns()
  {
    std::vector<uint8_t> result;

    auto isInResult = [&](uint8_t u)
    {
      for(int r = 0; r < sizeof(u) * 8; r++)
        if(std::count(result.begin(), result.end(), std::rotr(u, r)) > 0)
          return true;

      return false;
    };

    for(uint8_t i = 0; i < std::numeric_limits<uint8_t>::max(); i++)
      if(!isInResult(i))
        result.push_back(i);

    result.push_back(std::numeric_limits<uint8_t>::max());

    std::sort(result.begin(), result.end(),
              [](auto l, auto r)
              {
                auto pl = std::popcount(l);
                auto pr = std::popcount(r);
                if(pl != pr)
                  return pl < pr;
                return l < r;
              });

    std::vector<uint8_t> reverse;

    for(auto r = std::next(result.rbegin()); r != result.rend(); r++)
    {
      uint8_t n = (*r * 0x0202020202ULL & 0x010884422010ULL) % 1023;
      reverse.push_back(n);
    }

    result.insert(result.end(), reverse.begin(), reverse.end());

    std::vector<Core::Pattern> ret;

    for(auto p : result)
    {
      Core::Pattern r;

      uint64_t m = 1;

      for(size_t i = 0; i < 8; i++)
      {
        auto s = (p & (m << i)) ? Core::StepType::Trigger : Core::StepType::Empty;
        r[i] = r[i + 8] = r[i + 16] = r[i + 24] = r[i + 32] = r[i + 40] = r[i + 48] = r[i + 56] = s;
      }
      ret.push_back(r);
    }

    return ret;
  }
}