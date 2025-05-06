#include <cassert>
#include <raylib.h>
#include <string>
#include <plog/Log.h>
#include "system/field_map.h"

using std::string;


FieldMap::~FieldMap() {
  UnloadTexture(base);
}

void FieldMap::loadMap(string map_name) {
  PLOGI << "Loading map: '" << map_name << "'";
  ready = false;

  string base_path = "graphics/maps/" + map_name + ".png";
  PLOGD << "Base Texture Path: '" << base_path << "'";
  assert(FileExists(base_path.c_str()));

  UnloadTexture(base);
  base = LoadTexture(base_path.c_str());
  ready = true;
}

void FieldMap::draw() {
  DrawTexture(base, 0, 0, WHITE);
}
