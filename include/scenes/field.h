#pragma once
#include <string>
#include <vector>
#include <memory>
#include "base/scene.h"
#include "base/entity.h"
#include "data/field_event.h"
#include "system/field_handler.h"
#include "system/field_map.h"


class FieldScene : public Scene {
public:
  FieldScene();
  ~FieldScene();

  void mapLoadProcedure(std::string map_name, 
                        std::string *spawn_name = NULL);
  void setupActors();
  void setupMapTransitions();

  void update() override;
  void fieldEventHandling(std::unique_ptr<FieldEvent> &event);

  void draw() override;
private:
  FieldMap field;
  FieldEventHandler field_handler;

  Camera2D camera;
  Entity *camera_target;

  LoadMapEvent next_map;
  bool map_ready = false;

  std::vector<std::unique_ptr<Entity>> entities;
};
