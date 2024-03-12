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

    WHEN("Envelope is used")
    {
      WHEN("fade in position is set")
      {
        Core::FramePos fadeInPos = 12;
        core.getApi().setParameter({ 0 }, Core::ParameterId::EnvelopeFadeInPos, fadeInPos);

        THEN("first samples are zero")
        {
          dsp.getRealtimeApi().doAudio(out, numFrames, [](const auto &) {});
          for(size_t f = 0; f < fadeInPos; f++)
          {
            CHECK(out[f].main.left == 0);
          }

          AND_THEN("other samples are unchanged")
          {
            for(size_t f = fadeInPos; f < numFrames; f++)
            {
              CHECK(out[f].main.left == unprocessedSamples->at(f).left);
            }
          }
        }

        AND_WHEN("fadeInLen is set")
        {
          Core::FramePos fadeInLen = 8;
          core.getApi().setParameter({ 0 }, Core::ParameterId::EnvelopeFadeInLen, fadeInLen);

          THEN("first samples are still zero")
          {
            dsp.getRealtimeApi().doAudio(out, numFrames, [](const auto &) {});
            for(size_t f = 0; f < fadeInPos; f++)
            {
              CHECK(out[f].main.left == 0);
            }
          }

          AND_THEN("next samples are faded in")
          {
            dsp.getRealtimeApi().doAudio(out, numFrames, [](const auto &) {});
            CHECK(out[fadeInPos + 0].main.left == unprocessedSamples->at(fadeInPos + 0).left * 0.0f);
            CHECK(out[fadeInPos + 1].main.left == unprocessedSamples->at(fadeInPos + 1).left * 0.125f);
            CHECK(out[fadeInPos + 2].main.left == unprocessedSamples->at(fadeInPos + 2).left * 0.25f);
            CHECK(out[fadeInPos + 3].main.left == unprocessedSamples->at(fadeInPos + 3).left * 0.375f);
            CHECK(out[fadeInPos + 4].main.left == unprocessedSamples->at(fadeInPos + 4).left * 0.5f);
            CHECK(out[fadeInPos + 5].main.left == unprocessedSamples->at(fadeInPos + 5).left * 0.625f);
            CHECK(out[fadeInPos + 6].main.left == unprocessedSamples->at(fadeInPos + 6).left * 0.75f);
            CHECK(out[fadeInPos + 7].main.left == unprocessedSamples->at(fadeInPos + 7).left * 0.875f);
          }

          AND_WHEN("fade out is used")
          {
            Core::FramePos fadeOutPos = 100;
            Core::FramePos fadeOutLen = 8;
            core.getApi().setParameter({ 0 }, Core::ParameterId::EnvelopeFadeOutPos, fadeOutPos);
            core.getApi().setParameter({ 0 }, Core::ParameterId::EnvelopeFadeOutLen, fadeOutLen);

            dsp.getRealtimeApi().doAudio(out, numFrames, [](const auto &) {});
            CHECK(out[fadeOutPos + 0].main.left == unprocessedSamples->at(fadeOutPos + 0).left * 1.0f);
            CHECK(out[fadeOutPos + 1].main.left == unprocessedSamples->at(fadeOutPos + 1).left * 0.875f);
            CHECK(out[fadeOutPos + 2].main.left == unprocessedSamples->at(fadeOutPos + 2).left * 0.75f);
            CHECK(out[fadeOutPos + 3].main.left == unprocessedSamples->at(fadeOutPos + 3).left * 0.625f);
            CHECK(out[fadeOutPos + 4].main.left == unprocessedSamples->at(fadeOutPos + 4).left * 0.5f);
            CHECK(out[fadeOutPos + 5].main.left == unprocessedSamples->at(fadeOutPos + 5).left * 0.375f);
            CHECK(out[fadeOutPos + 6].main.left == unprocessedSamples->at(fadeOutPos + 6).left * 0.25f);
            CHECK(out[fadeOutPos + 7].main.left == unprocessedSamples->at(fadeOutPos + 7).left * 0.125f);
          }
        }
      }
    }
  }
}