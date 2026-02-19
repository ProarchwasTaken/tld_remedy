#pragma once
#include <nlohmann/json_fwd.hpp>
#include <raylib.h>
#include <string>
#include <memory>
#include <vector>
#include "enums.h"
#include "data/session.h"
#include "data/entity.h"
#include "data/line.h"


/* During the process of loading a Map, this class and the FieldScene work
 * hand in hand. The FieldMap is mainly responsible for reading an 
 * external map file, and parsing it into data that the FieldScene can 
 * understand; Data which will then be used to decide which entities 
 * should be created and where. The responsibility of creating these
 * entities is left up to the FieldScene.
 *
 * The FieldMap is also responsible for setting the Map's textures and
 * collisions. As well as setup common data that the game will use to
 * "remember" certain information between maps.*/
class FieldMap {
public:
  ~FieldMap();
  void loadMap(Session &session, std::string map_name, 
               std::string *spawn_name = NULL);
  void parseMapData(Session &session, std::string &map_name, 
                    std::string json_path, std::string *spawn_name);
  void setupCollision(nlohmann::json &layer_objects);

  void findSpawnpoints(nlohmann::json &layer_objects);
  void findSpawnpoints(nlohmann::json &layer_objects, 
                       std::string spawn_name);

  void findMapTransitions(nlohmann::json &layer_objects);
  void findPickups(Session &session, std::string &map_name, 
                   nlohmann::json &layer_objects);
  void findEnemies(Session &session, std::string &map_name, 
                   nlohmann::json &layer_objects);
  std::vector<Direction> parseEnemyRoutine(std::string &raw_routine);

  void findSavePoints(nlohmann::json &layer_objects);

  /* This function, along with setupCommonData both utilize TERRIFYING
   * pointer arithmetic, and must be handled with care.*/
  int activeObject(std::string map_name, int object_id, CommonData *begin,
                   int count);
  void setupCommonData(std::string map_name, int object_id,
                       CommonData *begin, int *count, int limit);

  void draw();
  void drawCollLines();

  static std::vector<Line> collision_lines;
  std::vector<std::unique_ptr<EntityData>> entity_queue;
private:
  Texture base;
};
