#pragma once
#include <memory>
#include <raylib.h>
#include "enums.h"
#include "base/actor.h"
#include "data/rect_ex.h"
#include "data/animation.h"
#include "data/actor_event.h"
#include "system/sprite_atlas.h"
#include "field/actors/player.h"



class CompanionActor : public Actor {
public:
  CompanionActor(Vector2 position, Direction direction);
  ~CompanionActor();

  static void setAtlas(CompanionID id);

  void evaluateEvent(std::unique_ptr<ActorEvent> &event) override;

  void update() override;
  bool playerObscured();
  bool shouldBeMoving();
  void setIdleSprite();
  void moveAnimation();

  void draw() override;

  static SpriteAtlas atlas;
  bool follow_player = true;
private:
  static RectEx DEF_BOUNDS;
  static RectEx DEF_COLLIS;

  PlayerActor *plr = NULL;
  bool moving = false;
  bool obscuring_player = false;

  float anim_move_speed = 0.25;
  Animation anim_down = {{0, 1, 2, 1}, anim_move_speed};
  Animation anim_right = {{3, 4, 5, 4}, anim_move_speed};
  Animation anim_up = {{6, 7, 8, 7}, anim_move_speed};
  Animation anim_left = {{9, 10, 11, 10}, anim_move_speed};
};
