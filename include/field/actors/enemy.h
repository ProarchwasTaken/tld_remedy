#pragma once
#include <raylib.h>
#include <vector>
#include "enums.h"
#include "base/actor.h"
#include "data/actor.h"
#include "data/rect_ex.h"
#include "field/actors/player.h"
#include "system/sprite_atlas.h"


class EnemyActor : public Actor {
public:
  EnemyActor(EnemyActorData &data);
  ~EnemyActor();
  void correctSightRect();
  
  void update() override;
  void normalLogic();
  bool sightCheck();

  void pursue();

  void directionRoutine();
  Rectangle *getIdleSprite();

  void draw() override;
  void drawDebug() override;

  static SpriteAtlas atlas;
  static int pursuing_enemy;
private:
  int object_id;
  TroopID troop_id;

  std::vector<Direction> routine;
  std::vector<Direction>::iterator current_direction;
  float routine_clock = 0.0;

  float speed;
  float movement_speed = 128;

  RectEx sight;
  float sight_range = 96;
  bool pursuing = false;

  PlayerActor *plr = NULL;
  float awaiting_deletion = false;
};
