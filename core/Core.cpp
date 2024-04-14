#include "Core.h"
#include "Mosaik.h"
#include "DataModel.h"

namespace Core
{
  Core::Core(Dsp::Api::Control::Interface &dsp, const Glib::RefPtr<Glib::MainContext> &ctx,
             std::unique_ptr<DataModel> dataModel)
      : m_dsp(dsp)
      , m_dataModel(dataModel ? std::move(dataModel) : std::make_unique<DataModel>())
      , m_api(std::make_unique<Api::Mosaik>(ctx, *m_dataModel, m_dsp))
  {
  }

  Core::~Core() = default;

  Api::Interface &Core::getApi() const
  {
    return *m_api;
  }

}