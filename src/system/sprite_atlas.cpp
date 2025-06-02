#include <fstream>
#include <nlohmann/json_fwd.hpp>
#include <nlohmann/json.hpp>
#include <cassert>
#include <string>
#include <raylib.h>
#include <plog/Log.h>
#include "game.h"
#include "system/sprite_atlas.h"

using std::string, nlohmann::json, nlohmann::basic_json, std::ifstream;


SpriteAtlas::SpriteAtlas(string sprite_group) {
  this->sprite_group = sprite_group;
}

SpriteAtlas::~SpriteAtlas() {
  reset();
}

void SpriteAtlas::setup() {
  PLOGI << "Setting up atlas: '" << sprite_group << "'";

  string sheet_path = "graphics/actors/" + sprite_group + ".png";
  assert(FileExists(sheet_path.c_str()));

  PLOGD << "Loading spritesheet.";
  Image image = LoadImage(sheet_path.c_str());
  ImageColorReplace(&image, Game::palette[0], {0, 0, 0, 0});

  atlas = LoadTextureFromImage(image);
  UnloadImage(image);

  string data_path = "data/spritesheets/" + sprite_group + ".json";
  assert(FileExists(data_path.c_str()));

  PLOGI << "Parse sprites frame data.";
  ifstream file(data_path);
  json atlas_data = json::parse(file);

  basic_json scale = atlas_data["meta"]["size"];
  source = {0, 0, scale["w"], scale["h"]};
  PLOGD << "Spritesheet Scale: {" << source.width << ", " << 
    source.height << "}";

  for (basic_json frame : atlas_data["frames"]) {
    string sprite_name = frame["filename"];

    float x = frame["frame"]["x"];
    float y = frame["frame"]["y"];
    float w = frame["frame"]["w"];
    float h = frame["frame"]["h"];

    PLOGD << "Loaded frame data for sprite: " << sprite_name;
    sprites.push_back({x, y, w, h});
  }

  PLOGI << "Finished setting up atlas.";
  file.close();
}

void SpriteAtlas::reset() {
  assert(user_count == 0);
  PLOGI << "Resetting sprite atlas: '" << sprite_group << "'";

  UnloadTexture(atlas);
  sprites.clear();
}

void SpriteAtlas::use() {
  if (user_count == 0) {
    setup();
  }

  user_count++;
}

void SpriteAtlas::release() {
  assert(user_count != 0);

  user_count--;

  if (user_count == 0) {
    release();
  }
}
