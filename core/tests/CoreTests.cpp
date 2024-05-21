#include <catch2/catch_all.hpp>
#include <core/Core.h>
#include <core/api/Interface.h>
#include <core/DataModel.h>
#include <dsp/Dsp.h>
#include <dsp/api/control/Interface.h>
#include <dsp/api/realtime/Interface.h>

TEST_CASE("serialize")
{
  Core::DataModel saved;
  saved.get<Core::ParameterId::GlobalVolume>() = -12.f;
  saved.get<Core::ParameterId::Gain>({ 0 }) = -16.f;
  saved.get<Core::ParameterId::Gain>({ 1 }) = -24.f;
  nlohmann::json foo = saved;

  Core::DataModel loaded;
  from_json(foo, loaded);
  CHECK(loaded.get<Core::ParameterId::GlobalVolume>() == -12.f);
  CHECK(loaded.get<Core::ParameterId::Gain>({ 0 }) == -16.f);
  CHECK(loaded.get<Core::ParameterId::Gain>({ 1 }) == -24.f);

  WHEN("model is loaded")
  {
    int counterGlobal = 0;
    int counterTile = 0;
    Core::DataModel loaded;
    loaded.get<Core::ParameterId::GlobalVolume>() = -13.0f;
    loaded.get<Core::ParameterId::Gain>({ 3 }) = -13.0f;

    Tools::ImmediateComputations c;
    c.add(
        [&]
        {
          float v = loaded.get<Core::ParameterId::GlobalVolume>();
          counterGlobal++;
        });

    c.add(
        [&]
        {
          float v = loaded.get<Core::ParameterId::Gain>({ 3 });
          counterTile++;
        });

    CHECK(counterGlobal == 1);
    CHECK(counterTile == 1);
    from_json(foo, loaded);

    CHECK(loaded.get<Core::ParameterId::GlobalVolume>() == -12.f);

    THEN("computations are triggered")
    {
      CHECK(counterGlobal == 2);
      CHECK(counterTile == 2);
    }
  }
}

TEST_CASE("snapshot")
{
  Core::DataModel d;
  d.get<Core::ParameterId::Gain>({ 1 }) = -24.f;
  d.saveSnapshot(4);
  d.get<Core::ParameterId::Gain>({ 1 }) = -12.f;

  CHECK(d.get<Core::ParameterId::Gain>({ 1 }) == -12.f);

  d.loadSnapshot(4);
  CHECK(d.get<Core::ParameterId::Gain>({ 1 }) == -24.f);
}

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