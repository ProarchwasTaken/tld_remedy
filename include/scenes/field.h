#pragma once
#include <string>
#include <vector>
#include <memory>
#include "enums.h"
#include "base/scene.h"
#include "base/field_sequence.h"
#include "base/entity.h"
#include "data/actor.h"
#include "data/field_event.h"
#include "data/actor_event.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
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
  void initPlayerData(SubWeaponID weapon_id);
  void initCompanionData(CompanionID companion_id); 
  void mapLoadProcedure(std::string map_name, 
                        std::string *spawn_name = NULL);
  void setupEntities();
  void setupActor(ActorData *data);

  void update() override;

  void panelLogic();
  void panelTermination();
  void dialogHandling();

  void actorBehavior();
  void eventEvaluation(std::unique_ptr<ActorEvent> &event);
  void eventProcessing();
  void eventHandling(std::unique_ptr<FieldEvent> &event);

  void initSequence(SequenceID sequence_id);
  void addStatusEffect(FieldEVT type, StatusID effect_id);
  void removeStatusEffect(FieldEVT type, StatusID effect_id);
  void addItem(ItemID item);
  static void removeItem(Session *session, ItemID item);
  void clearInventory();
  void updateCommonData(int object_id, bool active);
  void deleteEntity(int entity_id);

  void draw() override;

  #ifndef NDEBUG
  void drawSessionInfo();  
  #endif // !NDEBUG

  static SoundAtlas sfx;
  static SpriteAtlas emotes;
private:
  std::unique_ptr<Session> session;
  std::unique_ptr<FieldMap> field;
  std::unique_ptr<FieldSequence> sequence;
  Texture vignette;

  FieldHandler evt_handler;
  ActorHandler actor_handler;

  FieldCamera camera;
  Entity *camera_target;

  LoadMapEvent next_map;
  bool map_ready = false;

  std::vector<std::unique_ptr<Entity>> entities;
};
