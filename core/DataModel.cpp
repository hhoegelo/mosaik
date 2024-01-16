#include "DataModel.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace nlohmann
{
  template <> struct adl_serializer<Core::DataModel::Channel>
  {
    static void to_json(json &j, const Core::DataModel::Channel &v)
    {
      j = { { "sample", v.sample.string() }, { "pattern", v.pattern }, { "gain", v.gain },
            { "balance", v.balance },        { "muted", v.muted },     { "reverse", v.reverse } };
    }

    static void from_json(const json &j, Core::DataModel::Channel &v)
    {
      v.sample = std::filesystem::path((std::string) j["sample"]);
      v.pattern = j["pattern"];
      v.gain = j["gain"];
      v.balance = j["balance"];
      v.muted = j["muted"];
      v.reverse = j["reverse"];
    }
  };

  template <> struct adl_serializer<Core::DataModel>
  {
    static void to_json(json &j, const Core::DataModel &v)
    {
      j = { { "volume", v.volume }, { "tempo", v.tempo }, { "channels", v.channels } };
    }
  };
}

Core::DataModel::DataModel(const std::filesystem::path &f)
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
      channels = j["channels"];
    }
    catch(...)
    {
      std::cerr << "Sorry, could not read initial setup file." << std::endl;
    }
  }
}
Core::DataModel::~DataModel()
{
  std::ofstream o(backing);
  nlohmann::json j = *this;
  o << j;
}