#include "Mosaik.h"
#include <dsp/api/control/Interface.h>
#include <core/ParameterDescriptor.h>
#include <cmath>

#define JSON_ASSERT(x)
#include <tools/json.h>
#include <fstream>
#include <iostream>

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
                   for(uint8_t t = 0; t < NUM_TILES; t++)
                   {
                     if((address.tile != t) && std::get<bool>(core.getParameter({ t }, ParameterId::Selected)))
                     {
                       core.setParameter({ t }, ParameterId::Selected, false);
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
    bindParameters<GlobalParameterDescriptors>({}, m_model.globals);

    for(auto t = 0; t < NUM_TILES; t++)
    {
      Address a { t };
      bindParameters<TileParameterDescriptors>(a, model.tiles[t]);
      m_sanitizeSamplePositions.add([this, a] { sanitizeSamplePositions(a); });
    }

    init();
    m_kernelUpdate.add([this] { updateAudioKernel(); });
  }

  template <typename Description, typename Tuple> void Mosaik::bindParameter(Address address, Tuple &target)
  {
    m_access[{ address, Description::id }] = Binder<Description::id, typename Description::Type>::bind(
        *this, m_dsp, address, DataModel::get<Description::id>(address, target));
  }

  template <typename Parameters, typename Tuple> void Mosaik::bindParameters(Address address, Tuple &target)
  {
    std::apply([&](auto... a) { (bindParameter<decltype(a)>(address, target), ...); }, Parameters {});
  }

  void Mosaik::updateAudioKernel()
  {
    m_dsp.takeAudioKernel(newDspKernel(m_model));
    m_dsp.cleanupCache(getAllSamples(m_model));
  }

  void Mosaik::sanitizeSamplePositions(const Address &a) const
  {
    auto f = m_model.get<ParameterId::SampleFile>(a);
    auto s = m_dsp.getSamples(f);
    if(auto l = s->size())
    {
      auto &fadeIn = m_model.get<ParameterId::EnvelopeFadeInPos>(a);
      auto &fadedIn = m_model.get<ParameterId::EnvelopeFadedInPos>(a);
      auto &fadeOut = m_model.get<ParameterId::EnvelopeFadeOutPos>(a);
      auto &fadedOut = m_model.get<ParameterId::EnvelopeFadedOutPos>(a);
      auto &triggerFrame = m_model.get<ParameterId::TriggerFrame>(a);

      fadeIn = std::clamp<FramePos>(fadeIn, 0, l);
      fadedIn = std::clamp<FramePos>(fadedIn, 0, l);
      fadeOut = std::clamp<FramePos>(fadeOut, 0, l);
      fadedOut = std::clamp<FramePos>(fadedOut, 0, l);
      triggerFrame = std::clamp<FramePos>(triggerFrame, 0, l);
    }
  }

  const Mosaik::ParamAccess &Mosaik::findAccess(Address address, ParameterId parameterId) const
  {
    if(Core::GlobalParameters<Core::NoWrap>::contains(parameterId))
      address = {};

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
    auto num16thPerMinute
        = source.get<ParameterId::GlobalTempo>() * 4 * source.get<ParameterId::GlobalTempoMultiplier>();

    target->framesPer16th = numFramesPerMinute / num16thPerMinute;
    target->framesPerLoop = target->framesPer16th * NUM_STEPS;
    target->volume_dB = source.get<ParameterId::GlobalVolume>();
    target->prelistenVolume_dB = source.get<ParameterId::GlobalPrelistenVolume>();

    target->prelistenSample = m_dsp.getSamples(source.prelistenSample);
    target->prelistenInteractionCounter = source.prelistenInteractionCounter;

    target->reverbRoomSize = source.get<ParameterId::GlobalReverbRoomSize>();
    target->reverbColor = source.get<ParameterId::GlobalReverbColor>();

    target->reverbPreDelay = source.get<ParameterId::GlobalReverbPreDelay>();
    target->reverbChorus = source.get<ParameterId::GlobalReverbChorus>();
    target->reverbReturn = source.get<ParameterId::GlobalReverbReturn>();
    target->reverbOnOff = source.get<ParameterId::GlobalReverbOnOff>() == OnOffValues::On ? 1.0f : 0.0f;

    /* target->mainPlayground1 = source.globals.playground1;
    target->mainPlayground2 = source.globals.playground2;
    target->mainPlayground3 = source.globals.playground3;
    target->mainPlayground4 = source.globals.playground4;
    target->mainPlayground5 = source.globals.playground5;
    target->mainPlayground6 = source.globals.playground6;
    target->mainPlayground7 = source.globals.playground7;*/

    auto one = source.tappedOne.get();
    target->sequencerStartTime = one.has_value() ? one.value() : firstCompilation;
  }

  void Mosaik::translateTile(const DataModel &dataModel, Dsp::AudioKernel::Tile &tgt, const Address &src) const
  {
    auto numFramesPerMinute = SAMPLERATE * 60.0f;
    auto num16thPerMinute
        = dataModel.get<ParameterId::GlobalTempo>() * 4 * dataModel.get<ParameterId::GlobalTempoMultiplier>();
    auto framesPer16th = static_cast<Dsp::FramePos>(numFramesPerMinute / num16thPerMinute);
    auto framePerLoop = framesPer16th * 64;

    Dsp::FramePos pos = 0;

    tgt.audio = m_dsp.getSamples(dataModel.get<ParameterId::SampleFile>(src));

    for(auto s : dataModel.get<ParameterId::Pattern>(src))
    {
      if(s)
      {
        auto step = pos / framesPer16th;
        int64_t shuffle = (step % 2) ? shuffle
            = static_cast<int64_t>(0.5 * static_cast<double>(framesPer16th) * dataModel.get<ParameterId::Shuffle>(src))
                                     : 0;

        auto audioLen = tgt.audio->size();

        auto hitPoint = static_cast<FramePos>(dataModel.get<ParameterId::TriggerFrame>(src)
                                              / std::pow(2, dataModel.get<ParameterId::Speed>(src)));

        if(dataModel.get<ParameterId::Reverse>(src))
          hitPoint = audioLen - hitPoint;

        auto finalPos = pos + shuffle - hitPoint;

        while(finalPos < 0)
          finalPos += framePerLoop;

        tgt.triggers.push_back(finalPos % framePerLoop);
      }

      std::sort(tgt.triggers.begin(), tgt.triggers.end());

      pos += framesPer16th;
    }

    tgt.mute = dataModel.get<ParameterId::Mute>(src);
    tgt.balance = dataModel.get<ParameterId::Balance>(src);
    tgt.gain_dB = dataModel.get<ParameterId::Gain>(src);
    tgt.playbackFrameIncrement = powf(2.0f, dataModel.get<ParameterId::Speed>(src));
    tgt.reverse = dataModel.get<ParameterId::Reverse>(src);
    tgt.reverbSend_dB = dataModel.get<ParameterId::ReverbSend>(src);

    /*tgt.playground1 = src.playground1;
    tgt.playground2 = src.playground2;
    tgt.playground3 = src.playground3;
    tgt.playground4 = src.playground4;
    tgt.playground5 = src.playground5;
    tgt.playground6 = src.playground6;
    tgt.playground7 = src.playground7;*/

    auto calcM
        = [](float startY, float endY, FramePos l) { return l ? (endY - startY) / static_cast<float>(l) : 0.0f; };
    auto calcB = [&](float startY, float endY, FramePos p, FramePos l)
    { return startY - calcM(startY, endY, l) * static_cast<float>(p); };

    auto &fadedOutSection = tgt.envelope[0];
    auto &fadeOutSection = tgt.envelope[1];
    auto &fadedInSection = tgt.envelope[2];
    auto &fadeInSection = tgt.envelope[3];
    auto &preFadeInSection = tgt.envelope[4];

    fadedOutSection = { dataModel.get<ParameterId::EnvelopeFadedOutPos>(src), c_zeroDB, c_silenceDB };

    const auto fadeOutLen
        = dataModel.get<ParameterId::EnvelopeFadedOutPos>(src) - dataModel.get<ParameterId::EnvelopeFadeOutPos>(src);
    fadeOutSection = { dataModel.get<ParameterId::EnvelopeFadeOutPos>(src), calcM(c_zeroDB, c_silenceDB, fadeOutLen),
                       calcB(c_zeroDB, c_silenceDB, dataModel.get<ParameterId::EnvelopeFadeOutPos>(src), fadeOutLen) };

    fadedInSection = { dataModel.get<ParameterId::EnvelopeFadedInPos>(src), 0.0f, c_zeroDB };

    const auto fadeInLen
        = dataModel.get<ParameterId::EnvelopeFadedInPos>(src) - dataModel.get<ParameterId::EnvelopeFadeInPos>(src);
    fadeInSection = { dataModel.get<ParameterId::EnvelopeFadeInPos>(src), calcM(c_silenceDB, c_zeroDB, fadeInLen),
                      calcB(c_silenceDB, c_zeroDB, dataModel.get<ParameterId::EnvelopeFadeInPos>(src), fadeInLen) };

    preFadeInSection = { 0, 0, c_silenceDB };
  }

  Dsp::AudioKernel *Core::Api::Mosaik::newDspKernel(const DataModel &dataModel) const
  {
    auto r = std::make_unique<Dsp::AudioKernel>();
    translateGlobals(r.get(), dataModel);

    for(auto c = 0; c < NUM_TILES; c++)
    {
      auto &tgt = r->tiles[c];
      translateTile(dataModel, tgt, Address { c });
    }

    return r.release();
  }

  std::vector<Path> Mosaik::getAllSamples(DataModel &model) const
  {
    std::vector<Path> ret { model.prelistenSample };

    for(auto c = 0; c < NUM_TILES; c++)
      ret.push_back(model.get<ParameterId::SampleFile>({ c }));

    for(auto s = 0; s < NUM_SNAPSHOTS; s++)
      for(auto c = 0; c < NUM_TILES; c++)
        if(model.snapshots[s].has_value())
          ret.push_back(model.snapshots[s].value().get<ParameterId::SampleFile>({ c }));

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

  void Mosaik::loadSnapshot(int id)
  {
    m_model.loadSnapshot(id);
  }

  void Mosaik::saveSnapshot(int id)
  {
    m_model.saveSnapshot(id);
  }

  void Mosaik::load(const Path &path)
  {
    if(exists(path))
    {
      try
      {
        nlohmann::json j;
        std::ifstream(path) >> j;
        from_json(j, m_model);
      }
      catch(...)
      {
        std::cerr << "Could not read initial setup file." << std::endl;
      }
    }
  }

  void Mosaik::save(const Path &path)
  {
    nlohmann::json j = m_model;
    std::ofstream(path) << j;
  }
}
