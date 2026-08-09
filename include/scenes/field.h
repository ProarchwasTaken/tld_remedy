#pragma once
#include <cstddef>
#include <string>
#include <thread>
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
#include "system/sound_atlas.h"
#include "field/system/field_map.h"
#include "field/system/camera.h"
#include "field/system/field_handler.h"
#include "field/system/actor_handler.h"
#include "field/actors/player.h"
#include "field/actors/companion.h"


/* The main scene of the entire game. It's importance is not to be 
 * underestimated as it is a scene that must always exist in order for 
 * other scenes to function. (ie: CombatScene, CampMenuScene, and the
 * RestMenuScene) 
 *
 * So instead of being cleared from memory when the game loads a new 
 * scene, the FieldScene will be placed in "reserve" or "shelfed" until 
 * the game eventually returns to it. This is to make sure important
 * data like the player's session wouldn't become lost, and to also 
 * presevere minor stuff like the player's position on the map. 
 *
 * It also ensures that important data owned by the FieldScene would 
 * still be usable by other scenes. Provided that they are given pointer
 * to that data in the first place
 *
 * The FieldScene would only be cleared from memory when it's existence
 * is no longer needed. (IE: When the player wins, returns to the 
 * TitleScene, or gets a Game Over.)*/
class FieldScene : public Scene {
public:
  /* For when the player start a new game. Entirely new Session data
   * will be initialized, and the scene will be poised to load the
   * first map.*/
  FieldScene(SubWeaponID sub_weapon, CompanionID companion);

  /* For when the player loads an existing game. Basically takes 
   * existing session data that is stored enternally, and continues
   * where that session had left off.*/
  FieldScene(Session *session_data);

  /* For directly loading a map from startup with specific conditions. 
   * Purely meant for debug purposes as this constructor can only be ran 
   * through the use of command-line arguments.*/
  FieldScene(std::string map_name, SubWeaponID weapon, 
             CompanionID companion);
  ~FieldScene();

  void initPlayerData(SubWeaponID weapon_id);
  void initCompanionData(CompanionID companion_id);

  /* Only meant to ran on the scene's initialization. Basically
   * does some stuff to make sure the scene is ready to go. Y'know,
   * stuff like loading the map, setting up the debug command system,
   * loading the scene's sound effects and music.*/
  void setup(std::string starting_map);

  /* Called when the game returns to this scene after it was placed in
   * reserve. Useful for updating stuff that are not exactly important
   * enough to be updated every single frame, but not so inconsequencial
   * as to only be called once.*/
  void onSceneReturn(SceneID from);
  void deathsaveProcedure();
  float getPartySpeed();
  void updatePartySpeed(float multiplier);
  void updateInjury(Character &party_member);

  void loadMapProcedure(std::string map_name, 
                        std::string *spawn_name = NULL, 
                        bool map_history = true);
  void loadWrapup();
  void clearMapHistory();

  void setupEntities();
  void setupActor(ActorData *data);

  void update() override;

  void panelLogic();
  void panelTermination();
  void sequenceLogic();
  void dialogHandling();

  void actorBehavior();
  void eventEvaluation(std::unique_ptr<ActorEvent> &event);
  void eventProcessing();
  void eventHandling(std::unique_ptr<FieldEvent> &event);

  void initSequence(SequenceID sequence_id);
  void initSequence(SequenceID sequence_id, FlagID flag);

  void openDialog(OpenDialogEvent *event);
  void openDialogEx(OpenDialogEventEx *event);

  void addStatusEffect(FieldEVT type, StatusID effect_id);
  void removeStatusEffect(FieldEVT type, StatusID effect_id);

  void addItem(ItemID item);
  static void removeItem(Session *session, ItemID item);
  void clearInventory();

  void updateCommonData(int object_id, bool active);
  void markEnemyAsDead(int object_id);
  void reviveDeadEnemies();
  void deleteEntity(int entity_id);

  void draw() override;

  #ifndef NDEBUG
  void drawSessionInfo();  
  #endif // !NDEBUG

  static SoundAtlas sfx;
private:
  std::unique_ptr<Session> session;
  std::unique_ptr<FieldMap> field;
  std::unique_ptr<FieldSequence> sequence;
  Texture vignette;

  FieldHandler evt_handler;
  ActorHandler actor_handler;

  FieldCamera camera;
  PlayerActor *player_actor = NULL;
  CompanionActor *companion_actor = NULL;

  std::thread map_loading;
  LoadMapEvent next_map;
  bool init_load = false;

  std::vector<std::unique_ptr<Entity>> entities;
};
