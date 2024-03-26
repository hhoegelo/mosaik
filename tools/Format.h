#pragma once

#include <string>

namespace Tools
{
  template <typename... T> std::string format(const char *formatStr, T... a)
  {
    char txt[2048];
    snprintf(txt, sizeof(txt), formatStr, a...);
    return txt;
  }
}