#pragma once

#include <tools/ReactiveVar.h>
#include <core/ParameterDescriptor.h>
#include <concepts>

namespace Core
{
  template <ParameterId ID> using Reactive = Tools::ReactiveVar<typename ParameterDescriptor<ID>::Type>;

  template <ParameterId ID>
  concept HasDefault = requires() { ParameterDescriptor<ID>::defaultValue; };

  template <ParameterId ID> typename ParameterDescriptor<ID>::Type getDefaultValue()
  {
    return {};
  }

  template <ParameterId ID>
    requires HasDefault<ID>
  typename ParameterDescriptor<ID>::Type getDefaultValue()
  {
    return ParameterDescriptor<ID>::defaultValue;
  }

  template <ParameterId id> struct MakeReactive
  {
    using Wrapped = Reactive<id>;
  };

  template <ParameterId id> struct GetType
  {
    using Wrapped = ParameterDescriptor<id>::Type;
  };

  struct DataModel
  {
    DataModel(const DataModel &) = delete;
    DataModel &operator=(const DataModel &) = delete;

    using Globals = GlobalParameters<MakeReactive>::Wrapped;
    using Tile = TileParameters<MakeReactive>::Wrapped;
    using Tiles = std::array<Tile, NUM_TILES>;

    DataModel();

    template <ParameterId id> static auto &get(const Address &a, Tile &tile)
    {
      return TileParameters<NoWrap>::find<id>(tile);
    }

    template <ParameterId id> auto &get(const Address &a)
    {
      return get<id>(a, tiles[a.tile.value()]);
    }

    template <ParameterId id> auto &get(const Address &a) const
    {
      return TileParameters<NoWrap>::find<id>(tiles[a.tile.value()]).get();
    }

    template <ParameterId id> static auto &get(const Address &, Globals &g)
    {
      return GlobalParameters<NoWrap>::find<id>(g);
    }

    template <ParameterId id> auto &get()
    {
      return get<id>({}, globals);
    }

    template <ParameterId id> auto get() const
    {
      return GlobalParameters<NoWrap>::find<id>(globals).get();
    }

    void init();
    void saveSnapshot(int i);
    void loadSnapshot(int i);

    struct Snapshot
    {
      using Globals = GlobalParameters<GetType>::Wrapped;
      using Tile = TileParameters<GetType>::Wrapped;
      using Tiles = std::array<Tile, NUM_TILES>;

      Globals globals;
      Tiles tiles;

      template <ParameterId id> auto &get()
      {
        return GlobalParameters<NoWrap>::find<id>(globals);
      }

      template <ParameterId id> auto &get(const Address &a)
      {
        return TileParameters<NoWrap>::find<id>(tiles[a.tile.value()]);
      }
    };

    using Snapshots = std::array<std::optional<Snapshot>, NUM_SNAPSHOTS>;

    // persistent
    Globals globals;
    Tiles tiles;
    Snapshots snapshots;

    // temporary
    Tools::ReactiveVar<Path> prelistenSample { "" };
    Tools::ReactiveVar<uint8_t> prelistenInteractionCounter { 0 };
    Tools::ReactiveVar<std::optional<std::chrono::system_clock::time_point>> tappedOne;
  };

  void to_json(nlohmann::json &j, const DataModel &value);
  void from_json(const nlohmann::json &j, DataModel &value);
}
