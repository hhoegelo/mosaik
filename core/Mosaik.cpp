#include "Mosaik.h"
#include <dsp/api/control/Interface.h>
#include <core/ParameterDescriptor.h>
#include <cmath>

using namespace std::chrono_literals;

namespace Core::Api
{
  constexpr int c_silenceDB = -60.f;
  constexpr int c_zeroDB = 0.f;

  using IDsp = Dsp::Api::Control::Interface;
  using ICore = Interface;

  template <ParameterId id, typename V> struct Binder
  {
    static Mosaik::ParamAccess bind(ICore &, IDsp &, Address, Tools::ReactiveVar<V> &target)
    {
      using T = ParameterDescriptor<id>;
      return { .set = [&target](const auto &v) { target = std::get<V>(v); },
               .load = [&target](const auto &v) { target = std::get<V>(v); },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc = [](int) {} };
    }
  };

  template <ParameterId id> struct Binder<id, float>
  {
    static Mosaik::ParamAccess bind(ICore &, IDsp &, Address, Tools::ReactiveVar<float> &target)
    {
      using T = ParameterDescriptor<id>;
      return { .set = [&target](const auto &v) { target = std::clamp(std::get<float>(v), T::min, T::max); },
               .load = [&target](const auto &v) { target = std::get<float>(v); },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc = [&target](int steps)
               { target = std::clamp(target.get() + T::coarse * static_cast<float>(steps), T::min, T::max); } };
    }
  };

  template <ParameterId id> struct Binder<id, bool>
  {
    static Mosaik::ParamAccess bind(ICore &, IDsp &, Address, Tools::ReactiveVar<bool> &target)
    {
      using T = ParameterDescriptor<id>;
      return { .set = [&target](const auto &v) { target = std::get<bool>(v); },
               .load = [&target](const auto &v) { target = std::get<bool>(v); },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc =
                   [&target](int steps)
               {
                 if(steps % 1)
                   target = !target.get();
               } };
    }
  };

  template <> struct Binder<ParameterId::Selected, bool>
  {
    static Mosaik::ParamAccess bind(ICore &core, IDsp &, Address address, Tools::ReactiveVar<bool> &target)
    {
      using T = ParameterDescriptor<ParameterId::Selected>;
      return { .set =
                   [&target, &core, address](const auto &v)
               {
                 target = std::get<bool>(v);
                 if(target.get())
                 {
                   for(uint8_t c = 0; c < NUM_CHANNELS; c++)
                     for(uint8_t t = 0; t < NUM_TILES_PER_CHANNEL; t++)
                     {
                       if((address.tile != t || address.channel != c)
                          && std::get<bool>(core.getParameter({ c, t }, ParameterId::Selected)))
                       {
                         core.setParameter({ c, t }, ParameterId::Selected, false);
                       }
                     }
                 }
               },
               .load = [&target](const auto &v) { target = std::get<bool>(v); },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc = [](int) {} };
    }
  };

  template <> struct Binder<ParameterId::EnvelopeFadeInPos, FramePos>
  {
    static Mosaik::ParamAccess bind(ICore &core, IDsp &dsp, Address address, Tools::ReactiveVar<FramePos> &target)
    {
      using T = ParameterDescriptor<ParameterId::EnvelopeFadeInPos>;

      return { .set = [&target](const ParameterValue &v) { target = std::get<FramePos>(v); },
               .load = [&target](const auto &v) { target = std::get<FramePos>(v); },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc =
                   [&target, &core, &dsp, address](int steps)
               {
                 const auto sample = std::get<Path>(core.getParameter(address, ParameterId::SampleFile));
                 const auto numSamples = static_cast<FramePos>(dsp.getSamples(sample)->size());

                 const auto fadedInPos = std::clamp<FramePos>(
                     std::get<FramePos>(core.getParameter(address, ParameterId::EnvelopeFadedInPos)), 0, numSamples);

                 const auto fadeOutPos = std::clamp<FramePos>(
                     std::get<FramePos>(core.getParameter(address, ParameterId::EnvelopeFadeOutPos)), 0, numSamples);

                 const auto fadeInLen = fadedInPos - target;
                 const FramePos max = fadeOutPos - fadeInLen;

                 target = std::clamp<FramePos>(target + steps, 0, max);
                 core.loadParameter(address, ParameterId::EnvelopeFadedInPos,
                                    std::min(target.get() + fadeInLen, numSamples));
               } };
    }
  };

  template <> struct Binder<ParameterId::EnvelopeFadedInPos, FramePos>
  {
    static Mosaik::ParamAccess bind(ICore &core, IDsp &dsp, Address address, Tools::ReactiveVar<FramePos> &target)
    {
      using T = ParameterDescriptor<ParameterId::EnvelopeFadedInPos>;

      return { .set = [&target](const ParameterValue &v) { target = std::get<FramePos>(v); },
               .load = [&target](const auto &v) { target = std::get<FramePos>(v); },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc =
                   [&target, &core, &dsp, address](int steps)
               {
                 const auto sample = std::get<Path>(core.getParameter(address, ParameterId::SampleFile));
                 const auto numSamples = static_cast<FramePos>(dsp.getSamples(sample)->size());

                 const auto fadeInPos = std::clamp<FramePos>(
                     std::get<FramePos>(core.getParameter(address, ParameterId::EnvelopeFadeInPos)), 0, numSamples);

                 const auto fadeOutPos = std::clamp<FramePos>(
                     std::get<FramePos>(core.getParameter(address, ParameterId::EnvelopeFadeOutPos)), 0, numSamples);

                 target = std::clamp(target + steps, fadeInPos, fadeOutPos);
               } };
    }
  };

  template <> struct Binder<ParameterId::EnvelopeFadeOutPos, FramePos>
  {
    static Mosaik::ParamAccess bind(ICore &core, IDsp &dsp, Address address, Tools::ReactiveVar<FramePos> &target)
    {
      using T = ParameterDescriptor<ParameterId::EnvelopeFadeOutPos>;

      return { .set = [&target](const ParameterValue &v) { target = std::get<FramePos>(v); },
               .load = [&target](const auto &v) { target = std::get<FramePos>(v); },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc =
                   [&target, &core, &dsp, address](int steps)
               {
                 const auto sample = std::get<Path>(core.getParameter(address, ParameterId::SampleFile));
                 const auto numSamples = static_cast<FramePos>(dsp.getSamples(sample)->size());

                 const auto fadedInPos = std::clamp<FramePos>(
                     std::get<FramePos>(core.getParameter(address, ParameterId::EnvelopeFadedInPos)), 0, numSamples);
                 const auto fadedOutPos = std::clamp<FramePos>(
                     std::get<FramePos>(core.getParameter(address, ParameterId::EnvelopeFadedOutPos)), 0, numSamples);

                 target = std::clamp(target + steps, fadedInPos, fadedOutPos);
               } };
    }
  };

  template <> struct Binder<ParameterId::EnvelopeFadedOutPos, FramePos>
  {
    static Mosaik::ParamAccess bind(ICore &core, IDsp &dsp, Address address, Tools::ReactiveVar<FramePos> &target)
    {
      using T = ParameterDescriptor<ParameterId::EnvelopeFadedOutPos>;

      return { .set = [&target](const ParameterValue &v) { target = std::get<FramePos>(v); },
               .load = [&target](const auto &v) { target = std::get<FramePos>(v); },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc =
                   [&target, &core, &dsp, address](int steps)
               {
                 const auto sample = std::get<Path>(core.getParameter(address, ParameterId::SampleFile));
                 const auto max = static_cast<FramePos>(dsp.getSamples(sample)->size());

                 const auto fadedInPos
                     = std::get<FramePos>(core.getParameter(address, ParameterId::EnvelopeFadedInPos));
                 const auto fadeOutPos
                     = std::get<FramePos>(core.getParameter(address, ParameterId::EnvelopeFadeOutPos));
                 const auto fadedOutPos
                     = std::get<FramePos>(core.getParameter(address, ParameterId::EnvelopeFadedOutPos));
                 const auto fadeOutLen = fadedOutPos - fadeOutPos;
                 const FramePos min = fadedInPos + fadeOutLen;

                 target = std::clamp(target + steps, min, max);
                 core.loadParameter(address, ParameterId::EnvelopeFadeOutPos,
                                    std::clamp(target.get() - fadeOutLen, fadedInPos, target.get()));
               } };
    }
  };

  template <> struct Binder<ParameterId::TriggerFrame, FramePos>
  {
    static Mosaik::ParamAccess bind(ICore &core, IDsp &dsp, Address address, Tools::ReactiveVar<FramePos> &target)
    {
      using T = ParameterDescriptor<ParameterId::TriggerFrame>;

      return { .set = [&target](const ParameterValue &v) { target = std::get<FramePos>(v); },
               .load = [&target](const auto &v) { target = std::get<FramePos>(v); },
               .get = [&target]() -> ParameterValue { return target.get(); },
               .inc =
                   [&target, &core, &dsp, address](int steps)
               {
                 const FramePos min = 0;
                 const auto sample = std::get<Path>(core.getParameter(address, ParameterId::SampleFile));
                 const auto max = static_cast<FramePos>(dsp.getSamples(sample)->size());
                 target = std::clamp(target + steps, min, max);
               } };
    }
  };

  Mosaik::Mosaik(Glib::RefPtr<Glib::MainContext> ctx, DataModel &model, Dsp::Api::Control::Interface &dsp)
      : m_model(model)
      , m_dsp(dsp)
      , m_kernelUpdate(std::move(ctx), 0)
      , m_sanitizeSamplePositions(Glib::MainContext::get_default(), 1)
  {
    bindParameters<GlobalParameterDescriptors>(
        {}, m_model.globals.tempo, m_model.globals.volume, m_model.globals.prelistenVolume,
        m_model.globals.reverbRoomSize, m_model.globals.reverbColor, m_model.globals.reverbPreDelay,
        m_model.globals.reverbChorus, m_model.globals.reverbReturn, m_model.globals.reverbOnOff,
        m_model.globals.playground1, m_model.globals.playground2, m_model.globals.playground3,
        m_model.globals.playground4, m_model.globals.playground5, m_model.globals.playground6,
        m_model.globals.playground7);

    for(auto c = 0; c < NUM_CHANNELS; c++)
    {
      auto &channelSrc = m_model.channels[c];
      bindParameters<ChannelParameterDescriptors>({ c, {} }, channelSrc.onOff, channelSrc.volume,
                                                  channelSrc.delayPrePost, channelSrc.delaySend,
                                                  channelSrc.reverbPrePost, channelSrc.reverbSend);

      for(auto t = 0; t < NUM_TILES_PER_CHANNEL; t++)
      {
        auto &src = channelSrc.tiles[t];
        bindParameters<TileParameterDescriptors>(
            { c, t }, src.selected, src.sample, src.reverse, src.pattern, src.balance, src.gain, src.muted, src.speed,
            src.envelopeFadeInPos, src.envelopeFadedInPos, src.envelopeFadeOutPos, src.envelopeFadedOutPos,
            src.triggerFrame, src.shuffle, src.playground1, src.playground2, src.playground3, src.playground4,
            src.playground5, src.playground6, src.playground7);

        m_sanitizeSamplePositions.add(
            [&src, &dsp]
            {
              auto f = src.sample.get();
              auto s = dsp.getSamples(f);
              if(auto l = s->size())
              {
                src.envelopeFadeInPos = std::clamp<FramePos>(src.envelopeFadeInPos, 0, l);
                src.envelopeFadedInPos = std::clamp<FramePos>(src.envelopeFadedInPos, 0, l);
                src.envelopeFadeOutPos = std::clamp<FramePos>(src.envelopeFadeOutPos, 0, l);
                src.envelopeFadedOutPos = std::clamp<FramePos>(src.envelopeFadedOutPos, 0, l);
                src.triggerFrame = std::clamp<FramePos>(src.triggerFrame, 0, l);
              }
            });
      }
    }

    m_kernelUpdate.add(
        [this]
        {
          m_dsp.takeAudioKernel(newDspKernel(m_model));
          m_dsp.cleanupCache(getAllSamples(m_model));
        });
  }

  const Mosaik::ParamAccess &Mosaik::findAccess(Address address, ParameterId parameterId) const
  {
    if(Core::GlobalParameters<Core::NoWrap>::contains(parameterId))
      address = {};

    if(Core::ChannelParameters<Core::NoWrap>::contains(parameterId))
      address.tile = {};

    return m_access.find({ address, parameterId })->second;
  }

  void Mosaik::setParameter(Address address, ParameterId parameterId, const ParameterValue &v)
  {
    findAccess(address, parameterId).set(v);
  }

  void Mosaik::loadParameter(Address address, ParameterId parameterId, const ParameterValue &value)
  {
    findAccess(address, parameterId).load(value);
  }

  void Mosaik::incParameter(Address address, ParameterId parameterId, int steps)
  {
    findAccess(address, parameterId).inc(steps);
  }

  void Mosaik::setPrelistenSample(const Path &path)
  {
    m_model.prelistenSample = path;
    m_model.prelistenInteractionCounter = m_model.prelistenInteractionCounter + 1;
  }

  ParameterValue Mosaik::getParameter(Address address, ParameterId parameterId) const
  {
    return findAccess(address, parameterId).get();
  }

  Dsp::SharedSampleBuffer Mosaik::getSamples(Address address) const
  {
    return m_dsp.getSamples(get<std::filesystem::path>(getParameter(address, ParameterId::SampleFile)));
  }

  void Core::Api::Mosaik::translateGlobals(Dsp::AudioKernel *target, const DataModel &source) const
  {
    static auto firstCompilation = std::chrono::system_clock::now();

    auto numFramesPerMinute = SAMPLERATE * 60.0f;
    auto num16thPerMinute = source.globals.tempo * 4;

    target->framesPer16th = numFramesPerMinute / num16thPerMinute;
    target->framesPerLoop = target->framesPer16th * NUM_STEPS;
    target->volume_dB = source.globals.volume;
    target->prelistenVolume_dB = source.globals.prelistenVolume;

    target->prelistenSample = m_dsp.getSamples(source.prelistenSample);
    target->prelistenInteractionCounter = source.prelistenInteractionCounter;

    target->reverbRoomSize = source.globals.reverbRoomSize;
    target->reverbColor = source.globals.reverbColor;
    target->reverbPreDelay = source.globals.reverbPreDelay;
    target->reverbChorus = source.globals.reverbChorus;
    target->reverbReturn = source.globals.reverbReturn;
    target->reverbOnOff = source.globals.reverbOnOff == OnOffValues::On ? 1.0f : 0.0f;

    target->mainPlayground1 = source.globals.playground1;
    target->mainPlayground2 = source.globals.playground2;
    target->mainPlayground3 = source.globals.playground3;
    target->mainPlayground4 = source.globals.playground4;
    target->mainPlayground5 = source.globals.playground5;
    target->mainPlayground6 = source.globals.playground6;
    target->mainPlayground7 = source.globals.playground7;

    auto one = source.tappedOne.get();
    target->sequencerStartTime = one.has_value() ? one.value() : firstCompilation;
  }

  void Mosaik::translateChannel(const DataModel &dataModel, Dsp::AudioKernel::Channel &tgt,
                                const DataModel::Channel &src) const
  {
    tgt.volume_dB = src.volume;
    tgt.muteFactor = src.onOff == OnOffValues::On ? 1.0f : 0.0f;
    tgt.preReverbDb = src.reverbPrePost == PrePostValues::Pre ? src.reverbSend.get() : c_silenceDB;
    tgt.postReverbDb = src.reverbPrePost == PrePostValues::Post ? src.reverbSend.get() : c_silenceDB;
    tgt.preDelayDb = src.delayPrePost == PrePostValues::Pre ? src.reverbSend.get() : c_silenceDB;
    tgt.postDelayDb = src.delayPrePost == PrePostValues::Post ? src.reverbSend.get() : c_silenceDB;

    for(uint8_t t = 0; t < NUM_TILES_PER_CHANNEL; t++)
    {
      translateTile(dataModel, tgt.tiles[t], src.tiles[t]);
    }
  }

  void Mosaik::translateTile(const DataModel &dataModel, Dsp::AudioKernel::Channel::Tile &tgt,
                             const DataModel::Channel::Tile &src) const
  {
    auto numFramesPerMinute = SAMPLERATE * 60.0f;
    auto num16thPerMinute = dataModel.globals.tempo * 4;
    auto framesPer16th = static_cast<Dsp::FramePos>(numFramesPerMinute / num16thPerMinute);
    auto framePerLoop = framesPer16th * 64;

    Dsp::FramePos pos = 0;

    tgt.audio = m_dsp.getSamples(src.sample);

    for(auto s : src.pattern.get())
    {
      if(s)
      {
        auto step = pos / framesPer16th;
        int64_t shuffle
            = (step % 2) ? shuffle = static_cast<int64_t>(0.5 * static_cast<double>(framesPer16th) * src.shuffle) : 0;

        auto audioLen = tgt.audio->size();

        auto hitPoint = static_cast<FramePos>(src.triggerFrame / std::pow(2, src.speed));

        if(src.reverse.get())
          hitPoint = audioLen - hitPoint;

        auto finalPos = pos + shuffle - hitPoint;

        while(finalPos < 0)
          finalPos += framePerLoop;

        tgt.triggers.push_back(finalPos % framePerLoop);
      }

      std::sort(tgt.triggers.begin(), tgt.triggers.end());

      pos += framesPer16th;
    }

    tgt.mute = src.muted;
    tgt.balance = src.balance;
    tgt.gain_dB = src.gain;
    tgt.playbackFrameIncrement = powf(2.0f, src.speed);
    tgt.reverse = src.reverse;

    tgt.playground1 = src.playground1;
    tgt.playground2 = src.playground2;
    tgt.playground3 = src.playground3;
    tgt.playground4 = src.playground4;
    tgt.playground5 = src.playground5;
    tgt.playground6 = src.playground6;
    tgt.playground7 = src.playground7;

    auto calcM
        = [](float startY, float endY, FramePos l) { return l ? (endY - startY) / static_cast<float>(l) : 0.0f; };
    auto calcB = [&](float startY, float endY, FramePos p, FramePos l)
    { return startY - calcM(startY, endY, l) * static_cast<float>(p); };

    auto &fadedOutSection = tgt.envelope[0];
    auto &fadeOutSection = tgt.envelope[1];
    auto &fadedInSection = tgt.envelope[2];
    auto &fadeInSection = tgt.envelope[3];
    auto &preFadeInSection = tgt.envelope[4];

    fadedOutSection = { src.envelopeFadedOutPos, c_zeroDB, c_silenceDB };

    const auto fadeOutLen = src.envelopeFadedOutPos - src.envelopeFadeOutPos;
    fadeOutSection = { src.envelopeFadeOutPos, calcM(c_zeroDB, c_silenceDB, fadeOutLen),
                       calcB(c_zeroDB, c_silenceDB, src.envelopeFadeOutPos, fadeOutLen) };

    fadedInSection = { src.envelopeFadedInPos, 0.0f, c_zeroDB };

    const auto fadeInLen = src.envelopeFadedInPos - src.envelopeFadeInPos;
    fadeInSection = { src.envelopeFadeInPos, calcM(c_silenceDB, c_zeroDB, fadeInLen),
                      calcB(c_silenceDB, c_zeroDB, src.envelopeFadeInPos, fadeInLen) };

    preFadeInSection = { 0, 0, c_silenceDB };
  }

  Dsp::AudioKernel *Core::Api::Mosaik::newDspKernel(const DataModel &dataModel) const
  {
    auto r = std::make_unique<Dsp::AudioKernel>();
    translateGlobals(r.get(), dataModel);

    for(auto c = 0; c < NUM_CHANNELS; c++)
    {
      const auto &src = dataModel.channels[c];
      auto &tgt = r->channels[c];
      translateChannel(dataModel, tgt, src);
    }

    return r.release();
  }

  std::vector<Path> Mosaik::getAllSamples(DataModel &model) const
  {
    std::vector<Path> ret { model.prelistenSample };

    for(const auto &c : model.channels)
      for(const auto &t : c.tiles)
        ret.push_back(t.sample);

    return ret;
  }

  void Mosaik::addTap()
  {
    auto now = std::chrono::system_clock::now();

    if(!m_taps.empty() && (now - m_taps.back()) > 3s)
      m_taps.clear();

    m_taps.push_back(now);

    m_model.tappedOne = m_taps.front();

    if(m_taps.size() > 1)
    {
      auto dur = m_taps.back() - m_taps.front();
      auto numTaps = m_taps.size();
      auto durPerTapNS = std::chrono::duration_cast<std::chrono::nanoseconds>(dur / (numTaps - 1)).count();
      auto minuteNS = std::chrono::duration_cast<std::chrono::nanoseconds>(60s).count();
      float bpm = minuteNS / static_cast<float>(durPerTapNS);
      setParameter({}, ParameterId::GlobalTempo, bpm);
    }
  }

  void Mosaik::setOne()
  {
    m_model.tappedOne = std::chrono::system_clock::now();
  }

  template <typename Parameters, typename Targets, size_t idx>
  void Mosaik::bindParameter(Address address, Targets targets)
  {
    using D = typename std::tuple_element_t<idx, Parameters>;
    bindParameter<D::id>(address, std::get<idx>(targets));
  }

  template <ParameterId id, typename T> void Mosaik::bindParameter(Address address, Tools::ReactiveVar<T> &target)
  {
    m_access[{ address, id }] = Binder<id, T>::bind(*this, m_dsp, address, target);
  }

  template <typename Parameters, typename Targets, size_t... idx>
  void Mosaik::bindParameters(std::integer_sequence<size_t, idx...> int_seq, Address address, Targets targets)
  {
    (bindParameter<Parameters, Targets, idx>(address, targets), ...);
  }

  template <typename Parameters, typename... Args> void Mosaik::bindParameters(Address address, Args &...target)
  {
    using Indizes = std::make_index_sequence<std::tuple_size_v<Parameters>>;
    bindParameters<Parameters>(Indizes {}, address, std::make_tuple(std::ref(target)...));
  }
}