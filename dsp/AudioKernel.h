#pragma once

#include <dsp/Types.h>
#include <stdint.h>
#include <memory>
#include <vector>
#include <array>
#include <chrono>

namespace Dsp
{
  struct AudioKernel
  {
    std::chrono::system_clock::time_point sequencerStartTime;

    float volume_dB = 0.0;
    float prelistenVolume_dB = 0.0;
    
    FramePos framesPer16th = 1;
    FramePos framesPerLoop = 1;

    SharedSampleBuffer prelistenSample { std::make_shared<SampleBuffer>() };
    uint8_t prelistenInteractionCounter = 0;

    // Reverb
    float reverbRoomSize { 0 };
    float reverbColor { 0 };
    float reverbPreDelay { 0 };
    float reverbChorus { 0 };
    float reverbReturn { 0 };
    float reverbOnOff { 0 };

    // Playground
    float mainPlayground1 { 0 };
    float mainPlayground2 { 0 };
    float mainPlayground3 { 0 };
    float mainPlayground4 { 0 };
    float mainPlayground5 { 0 };
    float mainPlayground6 { 0 };
    float mainPlayground7 { 0 };

    struct Channel
    {
      struct Tile
      {
        SharedSampleBuffer audio { std::make_shared<SampleBuffer>() };
        std::vector<uint64_t> triggers;  // sample positions where to (re-)start the sample

        float balance;  // -1 ... 1
        float gain_dB;
        bool mute;

        bool reverse = false;
        float playbackFrameIncrement { 0 };

        // Playground
        float playground1 { 0 };
        float playground2 { 0 };
        float playground3 { 0 };
        float playground4 { 0 };
        float playground5 { 0 };
        float playground6 { 0 };
        float playground7 { 0 };

        // Envelope
        struct LinearInterpolation
        {
          FramePos pos {};
          float m {};
          float b {};
        };

        // envelope section, desc sorted
        std::array<LinearInterpolation, 5> envelope;  // faded-out, fade-out, faded-in, fade-in, pre fade-in
      };

      Tile tiles[NUM_TILES_PER_CHANNEL];
      float volume_dB { 0 };
      float preReverbDb = 0.0f;
      float postReverbDb = c_silenceDB;
      float preDelayDb = 0.0f;
      float postDelayDb = c_silenceDB;
      float muteFactor = 1.0f;
    };

    Channel channels[NUM_CHANNELS];
  };
}