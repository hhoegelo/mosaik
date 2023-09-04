#pragma once

namespace Core
{
  class Api;
}

namespace Dsp
{
  namespace Api
  {
    namespace Display
    {
      class Api;
    }
  }
}

namespace Ui
{
  namespace Touch
  {
    class Ui
    {
     public:
      Ui(Core::Api &core, Dsp::Api::Display::Api &dsp);
    };
  }
}