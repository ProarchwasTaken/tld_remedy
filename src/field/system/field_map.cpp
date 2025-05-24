#include <nlohmann/json_fwd.hpp>
#include <nlohmann/json.hpp>
#include <cassert>
#include <raylib.h>
#include <string>
#include <fstream>
#include <vector>
#include <cstddef>
#include <plog/Log.h>
#include "enums.h"
#include "data/actor.h"
#include "data/entity.h"
#include "data/line.h"
#include "field/system/field_map.h"

using std::string, std::ifstream, nlohmann::json, std::vector, 
nlohmann::basic_json;


vector<Line> FieldMap::collision_lines;

FieldMap::~FieldMap() {
  UnloadTexture(base);
  collision_lines.clear();
  actor_queue.clear();
  map_trans_queue.clear();
}

void FieldMap::loadMap(string map_name, string *spawn_name) {
  PLOGI << "Loading map: '" << map_name << "'";

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
  parseMapData(json_path, spawn_name);

  PLOGI << "Map: '" << map_name << "' has been loaded successfully."; 
}

void FieldMap::parseMapData(string json_path, string *spawn_name) {
  ifstream file(json_path);
  PLOGI << "Parsing map data...";
  json map_data = json::parse(file);

  for (basic_json layer : map_data["layers"]) {
    string layer_name = layer["name"];

    if (layer_name == "Collisions") {
      retrieveCollLines(layer["objects"]);
    }
    else if (layer_name == "Spawnpoints") {
      if (spawn_name == NULL) findSpawnpoints(layer["objects"]);
      else findSpawnpoints(layer["objects"], *spawn_name);
    }
    else if (layer_name == "MapTransitions") {
      findMapTransitions(layer["objects"]);
    }
  }

  file.close();
}

void FieldMap::retrieveCollLines(json &layer_objects) {
  PLOGI << "Loading collision lines...";
  collision_lines.clear();

  for (basic_json object : layer_objects) {
    int object_id = object["id"];
    PLOGD << "Object ID:" << object_id;

    float base_x = object["x"];
    float base_y = object["y"];

    vector<Vector2> vertices;
    PLOGD << "Retrieving line vertices...";

    for (basic_json line : object["polyline"]) {
      float x = line["x"];
      float y = line["y"];

      Vector2 vertex = {base_x + x, base_y + y};
      vertices.push_back(vertex);
    }

    int count = vertices.size();
    PLOGD << "Constructing lines | Vertex Count: " << count;
    if (count <= 1) {
      PLOGE << "Vertex count is too low!";
      continue;
    }

    for (int index = 0; index < (count - 1); index++) {
      int next_index = index + 1;
      Vector2 start = vertices[index];
      Vector2 end = vertices[next_index];

      Line line = {start, end};
      collision_lines.push_back(line);
    }
  }

  PLOGI << "Finished. Lines created: " << collision_lines.size();
}

void FieldMap::findSpawnpoints(json &layer_objects) {
  PLOGI << "Searching for initial spawn points.";

  bool found_initial = false;
  for (basic_json object : layer_objects) {
    float x = object["x"];
    float y = object["y"];
    Direction direction = Direction::DOWN;
    ActorType actor_type = ActorType::PLAYER;

    if (object.find("type") == object.end()) {
      PLOGD << "Found initial spawn point for the player.";
      PLOGD << "(X: " << x << ", Y: " << y << ")";
      ActorData actor_data = {{x, y}, direction, actor_type};

      actor_queue.push_back(actor_data);
      found_initial = true;
      break;
    }
  }

  assert(found_initial);
}

void FieldMap::findSpawnpoints(json &layer_objects, string spawn_name) {
  PLOGI << "Searching for transition spawn points with the same class" <<
    " as: '" << spawn_name << "'";

  bool found_transition = false;
  for (basic_json object : layer_objects) {
    float x = object["x"];
    float y = object["y"];

    Direction direction = Direction::DOWN;
    ActorType actor_type = ActorType::PLAYER;

    if (object.find("type") == object.end()) {
      continue;
    }

    string type_value = object["type"];
    if (type_value == spawn_name) {
      PLOGD << "Found transition spawn point for the player.";
      PLOGD << "(X: " << x << ", Y: " << y << ")";
      ActorData actor_data = {{x, y}, direction, actor_type};
      actor_queue.push_back(actor_data);

      found_transition = true;
      break;
    }
  }

  if (!found_transition) {
    PLOGE << "Failed to find transition spawn points!";
    PLOGD << "Resorting to search for initial spawnpoints.";
    findSpawnpoints(layer_objects);
  }
}

void FieldMap::findMapTransitions(json &layer_objects) {
  PLOGI << "Searching for map transition triggers...";
  for (basic_json object : layer_objects) { 
    float x = object["x"];
    float y = object["y"];
    float width = object["width"];
    float height = object["height"];

    Rectangle rect = {x, y, width, height};
    if (object.find("properties") == object.end()) {
      continue;
    }

    string map_dest;
    string spawn_dest;
    Direction direction;

    for (basic_json property : object["properties"]) {
      string property_name = property["name"];
      if (property_name == "map_dest") {
        map_dest = property["value"];
      }
      else if (property_name == "spawn_dest") {
        spawn_dest = property["value"];
      }
      else if (property_name == "direction") {
        direction = static_cast<Direction>(property["value"]);
      }
    }

    MapTransData data = {map_dest, spawn_dest, rect, direction};
    PLOGD << "Trigger Data: {Map Destination: '" << map_dest <<
      "' Spawnpoint: '" << spawn_dest << "' Direction: " << direction;
    map_trans_queue.push_back(data);
  }
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
