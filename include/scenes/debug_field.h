#pragma once
#include <string>
#include <vector>
#include <memory>
#include "base/scene.h"
#include "base/entity.h"
#include "system/field_handler.h"
#include "system/field_map.h"


class DebugField : public Scene {
public:
  DebugField();
  ~DebugField();

  void mapLoadProcedure(std::string map_name, 
                        std::string *spawn_name = NULL);
  void setupActors();

  void update() override;
  void fieldEventHandling(std::unique_ptr<FieldEvent> &event);

  void draw() override;
private:
  FieldMap field;
  FieldEventHandler field_handler;

  Camera2D camera;
  Entity *camera_target;

  std::vector<std::unique_ptr<Entity>> entities;
};
