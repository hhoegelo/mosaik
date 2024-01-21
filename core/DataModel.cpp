#include "DataModel.h"
#define JSON_ASSERT(x)
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

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

  template <> struct adl_serializer<Core::DataModel>
  {
    static void to_json(json &j, const Core::DataModel &v)
    {
      j = { { "volume", v.volume }, { "tempo", v.tempo }, { "tiles", v.tiles } };
    }
  };
}

namespace Core
{
  DataModel::DataModel(const std::filesystem::path &f)
      : backing(f)
  {
    if(exists(backing))
    {
      try
      {
        std::ifstream i(backing);
        nlohmann::json j;
        i >> j;
        volume = j["volume"];
        tempo = j["tempo"];
        tiles = j["tiles"];
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