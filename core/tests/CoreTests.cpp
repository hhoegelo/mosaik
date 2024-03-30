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
  Core::Core core(dsp.getControlApi(), Glib::MainContext::get_default(), std::make_unique<Core::DataModel>());
  core.getApi().setParameter({ 0 }, Core::ParameterId::SampleFile, bd);
  auto unprocessedSamples = dsp.getControlApi().getSamples(bd);
  auto numFrames = unprocessedSamples->size();
  Dsp::OutFrame out[(numFrames)];

  WHEN("pattern has no steps programmed")
  {
    core.getApi().setParameter({ 0 }, Core::ParameterId::Pattern, pattern);
    Tools::DeferredComputations::waitForAllScheduledComputationsDone();

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
    Tools::DeferredComputations::waitForAllScheduledComputationsDone();

    THEN("audio is reproduced without any changes")
    {
      dsp.getRealtimeApi().doAudio(out, numFrames, [](const auto &) {});
      for(size_t f = 0; f < numFrames; f++)
      {
        CHECK(out[f].main.left == unprocessedSamples->at(f).left);
        CHECK(out[f].main.right == unprocessedSamples->at(f).right);
      }
    }
  }

  WHEN("parameter GlobalVolume is set to -50 dB")
  {
    auto &api = core.getApi();
    api.setParameter({}, Core::ParameterId::GlobalVolume, -50.0f);

    AND_WHEN("GlobalVolume is incremented once")
    {
      api.incParameter({}, Core::ParameterId::GlobalVolume, 1);

      THEN("it is incremented by coarse step")
      {
        CHECK(std::get<float>(api.getParameter({}, Core::ParameterId::GlobalVolume)) == -49.5);
      }
    }

    AND_WHEN("GlobalVolume is incremented 5 times by one")
    {
      api.incParameter({}, Core::ParameterId::GlobalVolume, 1);
      api.incParameter({}, Core::ParameterId::GlobalVolume, 1);
      api.incParameter({}, Core::ParameterId::GlobalVolume, 1);
      api.incParameter({}, Core::ParameterId::GlobalVolume, 1);
      api.incParameter({}, Core::ParameterId::GlobalVolume, 1);

      THEN("it is incremented by coarse step")
      {
        CHECK(std::get<float>(api.getParameter({}, Core::ParameterId::GlobalVolume)) == -47.5);
      }
    }

    AND_WHEN("GlobalVolume is incremented by 10")
    {
      api.incParameter({}, Core::ParameterId::GlobalVolume, 10);

      THEN("it is incremented by coarse step")
      {
        CHECK(std::get<float>(api.getParameter({}, Core::ParameterId::GlobalVolume)) == -45.0);
      }
    }
  }
}