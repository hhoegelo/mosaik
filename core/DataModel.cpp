#include "DataModel.h"

namespace nlohmann
{
  template <typename Descriptions, typename Tuple> void toJson(json &j, const Tuple &t)
  {
    Descriptions::forEach(
        [&](auto a)
        {
          using D = decltype(a);

          if constexpr(requires() { D::name; })
          {
            auto &b = Descriptions::template find<D::id>(t);
            j[D::name] = b.get();
          }
        });
  }

  template <typename Descriptions, typename Tuple> void fromJson(const json &j, Tuple &t)
  {
    Descriptions::forEach(
        [&](auto a)
        {
          using D = decltype(a);

          if constexpr(requires() { D::name; })
          {
            auto &b = Descriptions::template find<D::id>(t);
            b = static_cast<typename D::Type>(j.at(D::name));
          }
        });
  }

  template <> struct adl_serializer<Core::DataModel::Tile>
  {
    static void to_json(json &j, const Core::DataModel::Tile &t)
    {
      toJson<Core::TileParameters<Core::NoWrap>>(j, t);
    }

    static void from_json(const json &j, Core::DataModel::Tile &t)
    {
      fromJson<Core::TileParameters<Core::NoWrap>>(j, t);
    }
  };

  template <> struct adl_serializer<Core::DataModel::Globals>
  {
    static void to_json(json &j, const Core::DataModel::Globals &t)
    {
      toJson<Core::GlobalParameters<Core::NoWrap>>(j, t);
    }

    static void from_json(const json &j, Core::DataModel::Globals &t)
    {
      fromJson<Core::GlobalParameters<Core::NoWrap>>(j, t);
    }
  };

  template <> struct adl_serializer<Core::DataModel::Tiles>
  {
    static void to_json(json &j, const Core::DataModel::Tiles &t)
    {
      for(auto &a : t)
        j.push_back(a);
    }

    static void from_json(const json &j, Core::DataModel::Tiles &t)
    {
      for(size_t i = 0; i < t.size(); i++)
        j[i].get_to(t[i]);
    }
  };
}

namespace Core
{
  void to_json(nlohmann::json &j, const std::optional<DataModel::Snapshot> &value)
  {
    if(value.has_value())
    {
      j["globals"] = value.value().globals;
      j["tiles"] = value.value().tiles;
    }
    else
    {
      j = nullptr;
    }
  }

  void from_json(const nlohmann::json &j, std::optional<DataModel::Snapshot> &value)
  {
    if(j.is_null())
    {
      value.reset();
    }
    else
    {
      DataModel::Snapshot s;
      j.at("globals").get_to(s.globals);
      j.at("tiles").get_to(s.tiles);
      value = s;
    }
  }

  void to_json(nlohmann::json &j, const DataModel &value)
  {
    j["globals"] = value.globals;
    j["tiles"] = value.tiles;
    j["snapshots"] = value.snapshots;
  }

  void from_json(const nlohmann::json &j, DataModel &value)
  {
    j.at("globals").get_to(value.globals);
    j.at("tiles").get_to(value.tiles);
    j.at("snapshots").get_to(value.snapshots);
  }

  DataModel::DataModel()
  {
    init();
  }

  void DataModel::init()
  {
    get<ParameterId::Selected>({ 0 }) = true;
  }

  void DataModel::saveSnapshot(int i)
  {
    Snapshot s;
    GlobalParameters<NoWrap>::forEach([&](auto a) { s.template get<decltype(a)::id>() = get<decltype(a)::id>(); });

    for(int t = 0; t < NUM_TILES; t++)
      TileParameters<NoWrap>::forEach([&](auto a)
                                      { s.template get<decltype(a)::id>({ t }) = get<decltype(a)::id>({ t }); });

    snapshots[i] = s;
  }

  void DataModel::loadSnapshot(int i)
  {
    if(snapshots[i].has_value())
    {
      Snapshot &s = snapshots[i].value();
      GlobalParameters<NoWrap>::forEach([&](auto a) { get<decltype(a)::id>() = s.template get<decltype(a)::id>(); });

      for(int t = 0; t < NUM_TILES; t++)
        TileParameters<NoWrap>::forEach([&](auto a)
                                        { get<decltype(a)::id>({ t }) = s.template get<decltype(a)::id>({ t }); });
    }
  }
}