#include "Api.h"

namespace Dsp
{
  namespace Api
  {
    namespace Control
    {
      Api::Api()
      {
      }

      Api::~Api() = default;

      void Api::init()
      {
        for(Col c = 0; c < NUM_COLS; c++)
          for(Row r = 0; r < NUM_ROWS; r++)
            m_matrix[c][r] = createChannel(c, r);
      }

      Channel *Api::getChannel(Col col, Row row) const
      {
        return m_matrix[col][row].get();
      }

      Channel::Channel(Col c, Row r)
      {
      }

      Channel::~Channel() = default;
    }
  }
}