#pragma once
#include <nlohmann/json_fwd.hpp>
#include <raylib.h>
#include <string>
#include <vector>
#include "data/actor.h"
#include "data/line.h"


class FieldMap {
public:
  ~FieldMap();
  void loadMap(std::string map_name);
  void parseMapData(std::string json_path);
  void retrieveCollLines(nlohmann::json &layer_objects);

  void findSpawnpoints(nlohmann::json &layer_objects);

  void draw();
  void drawCollLines();

  bool ready = false;
  static std::vector<Line> collision_lines;
  std::vector<ActorData> actor_queue;
private:
  Texture base;
};
