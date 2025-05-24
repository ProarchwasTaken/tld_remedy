#pragma once
#include <string>
#include <vector>
#include <memory>
#include "base/scene.h"
#include "base/entity.h"
#include "data/field_event.h"
#include "data/session.h"
#include "field/system/field_handler.h"
#include "field/system/field_map.h"


class FieldScene : public Scene {
public:
  FieldScene(Session *session_data = NULL);
  ~FieldScene();

  void mapLoadProcedure(std::string map_name, 
                        std::string *spawn_name = NULL);
  void setupActors();
  void setupMapTransitions();

  void update() override;
  void fieldEventHandling(std::unique_ptr<FieldEvent> &event);

  void draw() override;
  void drawSessionInfo();
private:
  FieldMap field;
  FieldEventHandler field_handler;
  Session session;

  Camera2D camera;
  Entity *camera_target;

  LoadMapEvent next_map;
  bool map_ready = false;

  std::vector<std::unique_ptr<Entity>> entities;
};
