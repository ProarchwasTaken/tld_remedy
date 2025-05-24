#pragma once
#include <nlohmann/json_fwd.hpp>
#include <raylib.h>
#include <string>
#include <memory>
#include <vector>
#include "data/entity.h"
#include "data/line.h"


class FieldMap {
public:
  ~FieldMap();
  void loadMap(std::string map_name, std::string *spawn_name = NULL);
  void parseMapData(std::string json_path, std::string *spawn_name);
  void retrieveCollLines(nlohmann::json &layer_objects);

  void findSpawnpoints(nlohmann::json &layer_objects);
  void findSpawnpoints(nlohmann::json &layer_objects, 
                       std::string spawn_name);

  void findMapTransitions(nlohmann::json &layer_objects);

  void draw();
  void drawCollLines();

  static std::vector<Line> collision_lines;
  std::vector<std::unique_ptr<EntityData>> entity_queue;
private:
  Texture base;
};
