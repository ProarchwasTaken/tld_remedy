#include <nlohmann/json_fwd.hpp>
#include <nlohmann/json.hpp>
#include <cassert>
#include <raylib.h>
#include <string>
#include <fstream>
#include <vector>
#include <plog/Log.h>
#include "data/line.h"
#include "system/field_map.h"

using std::string, std::ifstream, nlohmann::json, std::vector;


FieldMap::~FieldMap() {
  UnloadTexture(base);
  collision_lines.clear();
}

void FieldMap::loadMap(string map_name) {
  PLOGI << "Loading map: '" << map_name << "'";
  float start_time = GetTime();
  ready = false;

  string base_path = "graphics/maps/" + map_name + ".png";
  string json_path = "data/maps/" + map_name + ".tmj";
  PLOGD << "Base Texture Path: '" << base_path << "'";
  PLOGD << "Map Data Path: '" << json_path << "'";

  if (!FileExists(base_path.c_str())) {
    PLOGE << "'" << map_name + ".png" << "' not found!";
    throw;
  }
  else if (!FileExists(json_path.c_str())) {
    PLOGE << "'" << map_name + ".tmj" << "' not found!";
    throw;
  }

  UnloadTexture(base);
  base = LoadTexture(base_path.c_str());
  parseMapData(json_path);

  PLOGI << "Map: '" << map_name << "' has been loaded."; 
  PLOGD << "Load Time: " << GetTime() - start_time;
  ready = true;
}

void FieldMap::parseMapData(string json_path) {
  ifstream file(json_path);
  PLOGI << "Parsing map data...";
  json map_data = json::parse(file);

  for (auto layer : map_data["layers"]) {
    string layer_name = layer["name"];
    if (layer_name == "Collision") {
      retrieveCollLines(layer["objects"]);
    }
  }

  file.close();
}

void FieldMap::retrieveCollLines(json &layer_objects) {
  PLOGI << "Loading collision lines...";
  collision_lines.clear();

  for (auto object : layer_objects) {
    int object_id = object["id"];
    PLOGD << "Object ID:" << object_id;

    float base_x = object["x"];
    float base_y = object["y"];

    vector<Vector2> vertices;
    PLOGD << "Retrieving line vertices...";

    for (auto line : object["polyline"]) {
      float x = line["x"];
      float y = line["y"];

      Vector2 vertex = {base_x + x, base_y + y};
      vertices.push_back(vertex);
      PLOGD << "Vertex Added: (" << x << ", " << y << ")";
    }

    int count = vertices.size();
    PLOGD << "Constructing lines | Vertex Count: " << count;
    if (count <= 1) {
      PLOGE << "Vertex count is too low!";
      continue;
    }

    for (int index; index < (count - 1); index++) {
      PLOGI << "Interation: " << index;
      int next_index = index + 1;
      Vector2 start = vertices[index];
      Vector2 end = vertices[next_index];

      Line line = {start, end};
      collision_lines.push_back(line);
    }
  }

  PLOGI << "Finished. Lines created: " << collision_lines.size();
}

void FieldMap::draw() {
  DrawTexture(base, 0, 0, WHITE);
}

void FieldMap::drawCollLines() {
  for (Line line : collision_lines) {
    DrawCircleV(line.start, 2, ORANGE);
    DrawLineV(line.start, line.end, ORANGE);
  }
}
