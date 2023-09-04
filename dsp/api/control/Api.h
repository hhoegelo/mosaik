#pragma once

#include <stdint.h>
#include <string>
#include <memory>

namespace Dsp
{
  namespace Api
  {
    namespace Control
    {
      static constexpr auto NUM_COLS = 16;
      static constexpr auto NUM_ROWS = 16;
      using Row = uint8_t;
      using Col = uint8_t;
      using PathToSample = std::string;
      using ParamValue = float;

      class Channel
      {
       public:
        Channel(Col c, Row r);
        virtual ~Channel();

        enum class Parameter
        {
          Mute,
          Volume,
          Balance,
        };

        virtual void loadSample(const PathToSample &sample) = 0;
        virtual void setParameter(Parameter p, ParamValue newValue) = 0;
      };

      class Api
      {
       public:
        Api();
        virtual ~Api();

        void init();
        Channel *getChannel(Col col, Row row) const;

       private:
        virtual std::unique_ptr<Channel> createChannel(Col col, Row row) = 0;

        using Column = std::array<std::unique_ptr<Channel>, NUM_ROWS>;
        using Matrix = std::array<Column, NUM_COLS>;
        Matrix m_matrix;
      };
    }
  }
}