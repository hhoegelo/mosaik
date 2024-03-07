#pragma once

#include <memory>

namespace Dsp::Api::Control
{
  class Interface;
}

namespace Core
{
  struct DataModel;

  namespace Api
  {
    class Interface;
  }

  class Core
  {
   public:
    explicit Core(Dsp::Api::Control::Interface &dsp);
    ~Core();

    [[nodiscard]] Api::Interface &getApi() const;

   private:
    Dsp::Api::Control::Interface &m_dsp;
    std::unique_ptr<DataModel> m_dataModel;
    std::unique_ptr<Api::Interface> m_api;
  };
}