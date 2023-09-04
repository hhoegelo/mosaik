#pragma once

#include <memory>

namespace Dsp
{
  namespace Api
  {
    namespace Control
    {
      class Interface;
    }
  }
}

namespace Core
{
  struct DataModel;

  namespace Api
  {
    class Interface;
  }

  // holds all the data and logic that can be tweaked via UI
  // cares for persistancy,  manages DSP
  class Core
  {
   public:
    Core(Dsp::Api::Control::Interface &dsp);
    ~Core();

    Api::Interface &getApi() const;

   private:
    Dsp::Api::Control::Interface &m_dsp;
    std::unique_ptr<DataModel> m_dataModel;
    std::unique_ptr<Api::Interface> m_api;
  };
}