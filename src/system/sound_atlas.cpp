#include <fstream>
#include <nlohmann/json_fwd.hpp>
#include <nlohmann/json.hpp>
#include <cassert>
#include <string>
#include <raylib.h>
#include "system/sound_atlas.h"
#include <plog/Log.h>

using std::string, nlohmann::json, nlohmann::basic_json, std::ifstream;


SoundAtlas::SoundAtlas(string category) {
  this->category = category;
}

SoundAtlas::~SoundAtlas() {
  reset();
}

void SoundAtlas::setup() {
  PLOGI << "Loading all sounds of category: '" << category << "'";
  string data_path = "data/sfx/" + category + ".json";

  ifstream file(data_path);
  if (!file.is_open()) {
    PLOGE << "File not found!";
    file.close();
    throw;
  }

  json data = json::parse(file);
  for (basic_json sfx_data : data["sfx"]) { 
    string name = sfx_data["name"];
    string filename = sfx_data["filename"];

    sound_table.emplace(name, LoadSound(filename.c_str()));
    PLOGD << "Sound Loaded: '" << name << "'";
  }

  PLOGI << "Finished loading sounds.";
  file.close();
}

void SoundAtlas::reset() {
  assert(user_count == 0);
  PLOGI << "Resetting sound atlas: '" << category << "'";

  for (auto element : sound_table) { 
    UnloadSound(element.second);
  }

  sound_table.clear();
}

void SoundAtlas::use() {
  if (user_count == 0) {
    setup();
  }

  user_count++;
}

void SoundAtlas::release() {
  assert(user_count != 0);

  user_count--;

  if (user_count == 0) {
    reset();
  }
}
