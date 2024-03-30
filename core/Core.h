#pragma once

#include <memory>
#include <glibmm/refptr.h>
#include <glibmm/main.h>

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
    Core(Dsp::Api::Control::Interface &dsp, const Glib::RefPtr<Glib::MainContext> &ctx,
         std::unique_ptr<DataModel> dataModel = nullptr);
    ~Core();

    [[nodiscard]] Api::Interface &getApi() const;

   private:
    Dsp::Api::Control::Interface &m_dsp;
    std::unique_ptr<DataModel> m_dataModel;
    std::unique_ptr<Api::Interface> m_api;
  };
}