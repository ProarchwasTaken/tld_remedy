#pragma once
#include <list>
#include <utility>
#include <memory>
#include <raylib.h>
#include "enums.h"
#include "base/actor.h"
#include "data/animation.h"
#include "data/actor_event.h"
#include "system/sprite_atlas.h"
#include "field/actors/player.h"



class CompanionActor : public Actor {
public:
  CompanionActor(CompanionID id, Vector2 position, Direction direction);
  ~CompanionActor();
  void setupAtlas(CompanionID id);

  void evaluateEvent(std::unique_ptr<ActorEvent> &event) override;

  void update() override;
  void pathfind();
  Rectangle *getIdleSprite();
  void moveAnimation();

  void draw() override;
  void drawDebug() override;

  static SpriteAtlas atlas;
private:
  PlayerActor *plr = NULL;

  std::list<std::pair<Vector2, enum Direction>> move_points;
  bool moving = false;
  float movement_speed = 56.5;

  Animation anim_down = {{0, 1, 2, 1}, 0.25};
  Animation anim_right = {{3, 4, 5, 4}, 0.25};
  Animation anim_up = {{6, 7, 8, 7}, 0.25};
  Animation anim_left = {{9, 10, 11, 10}, 0.25};
};
