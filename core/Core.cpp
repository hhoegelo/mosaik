#include "Core.h"
#include "api/Interface.h"
#include "DataModel.h"
#include <api/control/Interface.h>

namespace Core
{
  namespace Api
  {
    class Mosaik : public Interface
    {
     public:
      Mosaik(DataModel *model, Dsp::Api::Control::Interface &dsp)
          : m_model(model)
          , m_dsp(dsp)
      {
      }

      ~Mosaik() override = default;

      void setParameter(Col col, Row row, ParameterId id, ParameterValue v) override
      {
        auto &channel = m_model->matrix[col][row];

        switch(id)
        {
          case ParameterId::Foo:
            channel.foo = v;
            m_dsp.updateSomeChannelParameter(col, row, channel.foo + channel.bar * channel.baz);
            break;

          case ParameterId::Bar:
            channel.bar = v;
            m_dsp.updateSomeChannelParameter(col, row, channel.foo + channel.bar * channel.baz);
            break;

          case ParameterId::Baz:
            channel.baz = v;
            m_dsp.updateSomeChannelParameter(col, row, channel.foo + channel.bar * channel.baz);
            m_dsp.alsoDoSomethingElseForTheWholeColumn(col);
            break;

          default:
            throw std::invalid_argument("parameter id does not address a channel parameter");
        }

        commit(col, row, id, v);
      }

      void setParameter(ParameterId id, ParameterValue v) override
      {
        switch(id)
        {
          case ParameterId::MainVolume:
            m_model->mainVolume = v;
            break;

          case ParameterId::Tempo:
            m_model->bpm = v;
            break;

          default:
            throw std::invalid_argument("parameter id does not address a global parameter");
        }

        commit(id, v);
      }

      void loadSample(Col col, Row row, const Path &path) override
      {
        auto &channel = m_model->matrix[col][row];
        channel.sample = path;
        commit(col, row, path);
      }

     private:
      DataModel *m_model;
      Dsp::Api::Control::Interface &m_dsp;
    };
  }

  Core::Core(Dsp::Api::Control::Interface &dsp)
      : m_dsp(dsp)
      , m_dataModel(std::make_unique<DataModel>())
      , m_api(std::make_unique<Api::Mosaik>(m_dataModel.get(), m_dsp))
  {
  }

  Core::~Core() = default;

  Api::Interface &Core::getApi() const
  {
    return *m_api;
  }

}