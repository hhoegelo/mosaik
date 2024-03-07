#include "DataModel.h"
#define JSON_ASSERT(x)
#include <tools/json.h>
#include <fstream>
#include <iostream>
#include <utility>

namespace nlohmann
{
  template <> struct adl_serializer<Core::DataModel::Tile>
  {
    static void to_json(json &j, const Core::DataModel::Tile &v)
    {
      j = { { "sample", v.sample.string() }, { "pattern", v.pattern }, { "gain", v.gain },
            { "balance", v.balance },        { "muted", v.muted },     { "reverse", v.reverse },
            { "selected", v.selected } };
    }

    static void from_json(const json &j, Core::DataModel::Tile &v)
    {
      v.sample = std::filesystem::path((std::string) j["sample"]);
      v.pattern = j["pattern"];
      v.gain = j["gain"];
      v.balance = j["balance"];
      v.muted = j["muted"];
      v.reverse = j["reverse"];
      v.selected = j["selected"];
    }
  };

  template <> struct adl_serializer<Core::DataModel::Globals>
  {
    static void to_json(json &j, const Core::DataModel::Globals &v)
    {
      j = { { "volume", v.volume }, { "tempo", v.tempo } };
    }

    static void from_json(const json &j, Core::DataModel::Globals &v)
    {
      v.volume = j["volume"];
      v.tempo = j["tempo"];
    }
  };

  template <> struct adl_serializer<Core::DataModel>
  {
    static void to_json(json &j, const Core::DataModel &v)
    {
      j = { { "globals", v.globals }, { "tiles", v.tiles } };
    }

    static void from_json(const json &j, Core::DataModel &v)
    {
      v.globals = j["globals"];
      v.tiles = j["tiles"];
    }
  };
}

namespace Core
{
  DataModel::DataModel(std::filesystem::path f)
      : backing(std::move(f))
  {
    tiles[0].selected = true;

    if(exists(backing))
    {
      try
      {
        std::ifstream i(backing);
        nlohmann::json j;
        i >> j;
        nlohmann::adl_serializer<Core::DataModel>::from_json(j, *this);
      }
      catch(...)
      {
        std::cerr << "Could not read initial setup file." << std::endl;
      }
    }

    for(auto c = 0; c < NUM_TILES; c++)
      tiles[c].id = c;
  }

  DataModel::~DataModel()
  {
    std::ofstream o(backing);
    nlohmann::json j = *this;
    o << j;
  }
}