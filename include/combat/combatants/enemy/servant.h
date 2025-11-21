#pragma once
#include <raylib.h>
#include "enums.h"
#include "base/enemy.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "combat/actions/attack.h"


enum class ServantGoals {
  IDLE = 0,
  TARGETING = 1,
};


class Servant : public Enemy {
public:
  Servant(Vector2 position, Direction direction);
  ~Servant();

  void behavior() override;
  void rootBehavior();
  void chooseTarget();

  void setGoal(ServantGoals, float chance);

  void decideAttack();
  void attackMP();
  void attackHP();

  void update() override;
  void neutralLogic();
  void targetingLogic();

  void wait(float time);
  void wait(float min, float max);
  void waitTimer();

  void movement();

  void useMovingAnimation();
  Rectangle *getStunSprite();
  void draw() override;
  void drawDebug() override;

  static SpriteAtlas atlas;

  ServantGoals ai_goal = ServantGoals::IDLE;
private:
  const float default_speed = 64;
  int moving_x = 0;
  bool has_moved = false;

  float attack_distance = 32;

  bool waiting = false;
  float wait_time = 0.20;
  float wait_clock = 0.0;

  float anim_move_speed = 0.2125;
  Animation anim_move = {{1, 2, 3, 2}, anim_move_speed};

  Animation anim_dead = {{5, 6}, 0.5};

  AtkAnimSet atk_mp_set = {
    {{7, 8}, 0.05},
    {{10}, 0.05},
    9
  };

  AtkAnimSet atk_hp_set = {
    {{11}, 0.0},
    {{13, 14}, 0.0},
    12
  };
};
