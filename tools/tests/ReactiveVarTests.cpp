#include <catch2/catch_all.hpp>
#include <tools/ReactiveVar.h>

using namespace Tools;

TEST_CASE("ReactiveVar")
{
  int invocationCount = 0;

  ReactiveVar<int> v(1);

  ImmediateComputations computations;
  computations.add(
      [&]
      {
        int readOut = v;
        invocationCount++;
      });

  WHEN("Var is changed")
  {
    v = 2;

    THEN("Computation is invalidated")
    {
      CHECK(invocationCount == 2);
    }

    AND_WHEN("Nested computations are introduced")
    {
      int nextedInvocationCount = 0;
      ImmediateComputations nestedCmputations;
      nestedCmputations.add(
          [&]
          {
            int readOut = v;
            nextedInvocationCount++;
          });

      AND_WHEN("Var is changed again")
      {
        v = 4;

        THEN("both computations are invalidated")
        {
          CHECK(invocationCount == 3);
          CHECK(nextedInvocationCount == 2);
        }
      }
    }
  }

  WHEN("New Computations are done and destroyed")
  {
    int inv = 0;

    {
      ImmediateComputations nestedComputations;
      nestedComputations.add(
          [&]
          {
            int readOut = v;
            inv++;
          });
    }

    CHECK(inv == 1);

    v = v + 1;
    THEN("callback is not anymore called")
    {
      CHECK(inv == 1);
    }
  }
}