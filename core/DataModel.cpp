#include "DataModel.h"
#define JSON_ASSERT(x)
#include <tools/json.h>
#include <fstream>
#include <iostream>
#include <utility>

namespace nlohmann
{
  template <typename T> void loadIfExists(T &t, const json &j, const char *name)
  {
    if(j.contains(name))
      t = j[name];
  }

  template <typename T> struct adl_serializer<Tools::ReactiveVar<T>>
  {
    static void to_json(json &j, const Tools::ReactiveVar<T> &v)
    {
      j = v.get();
    }
  };

  template <> struct adl_serializer<Core::DataModel::Tile>
  {
    static void to_json(json &j, const Core::DataModel::Tile &v)
    {
      j = { { "sample", v.sample.get().string() },
            { "pattern", v.pattern },
            { "gain", v.gain },
            { "balance", v.balance },
            { "muted", v.muted },
            { "reverse", v.reverse },
            { "selected", v.selected },
            { "envelopeFadeInPos", v.envelopeFadeInPos },
            { "envelopeFadeInLen", v.envelopeFadeInLen },
            { "envelopeFadeOutPos", v.envelopeFadeOutPos },
            { "envelopeFadeOutLen", v.envelopeFadeOutLen },
            { "speed", v.speed } };
    }

    static void from_json(const json &j, Core::DataModel::Tile &v)
    {
      v.sample = std::filesystem::path((std::string) j["sample"]);
      loadIfExists(v.pattern, j, "pattern");
      loadIfExists(v.gain, j, "gain");
      loadIfExists(v.balance, j, "balance");
      loadIfExists(v.muted, j, "muted");
      loadIfExists(v.reverse, j, "reverse");
      loadIfExists(v.selected, j, "selected");

      loadIfExists(v.envelopeFadeInPos, j, "envelopeFadeInPos");
      loadIfExists(v.envelopeFadeInLen, j, "envelopeFadeInLen");
      loadIfExists(v.envelopeFadeOutPos, j, "envelopeFadeOutPos");
      loadIfExists(v.envelopeFadeOutLen, j, "envelopeFadeOutLen");

      loadIfExists(v.speed, j, "speed");
    }
  };

  template <> struct adl_serializer<Core::DataModel::Globals>
  {
    static void to_json(json &j, const Core::DataModel::Globals &v)
    {
      j = { { "volume", v.volume }, { "tempo", v.tempo }, { "shuffle", v.shuffle } };
    }

    static void from_json(const json &j, Core::DataModel::Globals &v)
    {
      v.volume = j["volume"];
      v.tempo = j["tempo"];
      v.shuffle = j["shuffle"];
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
  DataModel::DataModel() = default;

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