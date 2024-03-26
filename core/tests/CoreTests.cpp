#include <catch2/catch_all.hpp>
#include <core/Core.h>
#include <core/api/Interface.h>
#include <core/DataModel.h>
#include <dsp/Dsp.h>
#include <dsp/api/control/Interface.h>
#include <dsp/api/realtime/Interface.h>

TEST_CASE("translate core to dsp")
{
  Dsp::Dsp dsp;

  auto bd = SOURCE_DIR "/core/tests/bd.flac";
  Core::Pattern pattern { false };
  Core::Core core(dsp.getControlApi(), std::make_unique<Core::DataModel>());
  core.getApi().setParameter({ 0 }, Core::ParameterId::SampleFile, bd);
  auto unprocessedSamples = dsp.getControlApi().getSamples(bd);
  auto numFrames = unprocessedSamples->size();
  Dsp::OutFrame out[(numFrames)];

  WHEN("pattern has no steps programmed")
  {
    core.getApi().setParameter({ 0 }, Core::ParameterId::Pattern, pattern);
    dsp.getRealtimeApi().doAudio(out, numFrames, [](const auto &) {});

    THEN("no audio is produced")
    {
      for(size_t f = 0; f < numFrames; f++)
      {
        CHECK(out[f].main.left == 0);
        CHECK(out[f].main.right == 0);
      }
    }
  }

  WHEN("pattern has first step programmed")
  {
    pattern[0] = true;
    core.getApi().setParameter({ 0 }, Core::ParameterId::Pattern, pattern);

    THEN("no audio is reproduced without any changes")
    {
      dsp.getRealtimeApi().doAudio(out, numFrames, [](const auto &) {});
      for(size_t f = 0; f < numFrames; f++)
      {
        CHECK(out[f].main.left == unprocessedSamples->at(f).left);
        CHECK(out[f].main.right == unprocessedSamples->at(f).right);
      }
    }
  }
}