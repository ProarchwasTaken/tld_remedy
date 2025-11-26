#pragma once
#include <memory>
#include <raylib.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/enemy.h"
#include "data/animation.h"
#include "data/ai_behavior.h"
#include "data/combatant_event.h"
#include "system/sprite_atlas.h"
#include "combat/actions/attack.h"
#include "combat/actions/ghost_step.h"


enum class ServantGoals {
  IDLE = 0,
  TARGETING = 1,
  RETREATING = 2,
  DODGING = 3
};


struct ServantAI : AIBehavior {
  ServantAI() {
    ct_retreat_chance = 0.40;
    ct_min_retreat = 0.10;
    ct_max_retreat = 0.40;

    ct_wait_chance = 0.40;
    ct_min_wait = 0.10;
    ct_max_wait = 0.25;

    tg_retreat_chance = 0.45;
    tg_min_retreat = 0.10;
    tg_max_retreat = 0.75;

    rt_target_chance = 0.75;
    rt_min_wait = 0.10;
    rt_max_wait = 0.50;

    dg_target_chance = 0.80;
    dg_min_wait = 0.25;
    dg_max_wait = 0.50;

    dg_range_multiplier = 0.5;
    dg_time_multiplier = 1.0;
    dg_penalty = 0.5;
    dg_retailiation_chance = 0.80;
  }
};


class Servant : public Enemy {
public:
  Servant(Vector2 position, Direction direction);
  ~Servant();

  void behavior() override;
  void evaluateEvent(std::unique_ptr<CombatantEvent> &event) override;
  void warningHandling(WarningCBT *event);
  void retaliation(Combatant *assailant, float chance);
  float chanceCalculation(WarningCBT *event, bool from_target, 
                          bool in_range);

  void rootBehavior();
  void targetingBehavior();
  void chooseTarget();

  void setGoal(ServantGoals, float chance);

  void decideAttack();
  void attackMP();
  void attackHP();
  void ghoststep();

  void update() override;
  void neutralLogic();
  void targetingLogic();
  void retreatingLogic();
  void dodgingLogic();

  void wait(float time);
  void wait(float min, float max);
  void waitTimer();

  void movement();

  void useMovingAnimation();
  Rectangle *getStunSprite();
  void draw() override;
  void drawDebug() override;

  ServantGoals ai_goal = ServantGoals::IDLE;
  AIBehavior ai_behavior = ServantAI(); 
  float tick_clock = 0.0;

  static SpriteAtlas atlas;
private:
  const float default_speed = 64;
  int moving_x = 0;
  bool has_moved = false;

  float attack_distance = 24;
  float contest_distance = 128;

  float retreat_time = 0.5;
  float retreat_clock = 0.0;

  float dodge_time = 0.0;
  float dodge_clock = 0.0;

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

  GSSpriteSet gs_set = {15, 17, 16};
};
