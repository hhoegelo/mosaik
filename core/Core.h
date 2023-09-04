#pragma once

#include <memory>

namespace Dsp
{
  namespace Api
  {
    namespace Control
    {
      class Api;
    }
  }
}

namespace Core
{
  class Api;

  // holds all the data and logic that can be tweaked via UI
  // cares for persistancy,  manages DSP
  class Core
  {
   public:
    Core(Dsp::Api::Control::Api &dsp);
    ~Core();

    Api &getApi() const;

   private:
    std::unique_ptr<Api> m_api;
  };
}