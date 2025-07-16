#pragma once
#include <string>
#include <vector>
#include <memory>
#include "enums.h"
#include "base/scene.h"
#include "base/entity.h"
#include "data/actor.h"
#include "data/field_event.h"
#include "data/session.h"
#include "system/sound_atlas.h"
#include "field/system/field_map.h"
#include "field/system/camera.h"
#include "field/system/field_handler.h"
#include "field/system/actor_handler.h"


class FieldScene : public Scene {
public:
  FieldScene(SubWeaponID sub_weapon, CompanionID companion);
  FieldScene(Session *session_data);
  ~FieldScene();

  void setup();
  void initCompanionData(CompanionID id); 
  void mapLoadProcedure(std::string map_name, 
                        std::string *spawn_name = NULL);
  void setupEntities();
  void setupActor(ActorData *data);

  void update() override;
  void eventProcessing();
  void eventHandling(std::unique_ptr<FieldEvent> &event);
  void updateCommonData(int object_id, bool active);
  void deleteEntity(int entity_id);

  void draw() override;
  void drawSessionInfo();

  static SoundAtlas sfx;
private:
  FieldMap field;
  Texture vignette;

  FieldHandler evt_handler;
  ActorHandler actor_handler;
  Session session;

  FieldCamera camera;
  Entity *camera_target;

  LoadMapEvent next_map;
  bool map_ready = false;

  std::vector<std::unique_ptr<Entity>> entities;
};
