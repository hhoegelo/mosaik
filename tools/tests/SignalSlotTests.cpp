#include <catch2/catch.hpp>
#include <SignalSlot.h>

using namespace Tools::Signals;

TEST_CASE("SignalSlot")
{
  WHEN("Function is connected to signal")
  {
    bool called = false;
    Signal<void()> sig;
    auto connection = sig.connect([&] { called = true; });

    THEN("Emitting the signal calls the function")
    {
      sig.emit();
      CHECK(called == true);
    }

    AND_WHEN("connection is dropped")
    {
      connection.reset();

      THEN("Emitting the signal does not call the function")
      {
        sig.emit();
        CHECK(called == false);
      }
    }

    AND_WHEN("connection is copied before being dropped")
    {
      auto copy = connection;
      connection.reset();

      THEN("Emitting the signal calls the function")
      {
        sig.emit();
        CHECK(called == true);
      }
    }
  }

  WHEN("Function is connected to signal")
  {
    bool calledFirst = false;
    bool calledSecond = false;

    Signal<void()> sig;
    auto connection = sig.connect([&] { calledFirst = true; });

    AND_THEN("The connection is used to connect to something else")
    {
      connection = sig.connect([&] { calledSecond = true; });

      THEN("Emitting the signal calls the second function only")
      {
        sig.emit();
        CHECK(calledFirst == false);
        CHECK(calledSecond == true);
      }
    }
  }
}