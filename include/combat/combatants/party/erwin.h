#pragma once
#include <raylib.h>
#include "base/party_member.h"
#include "data/session.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"
#include "combat/actions/attack.h"


enum class ErwinGoals {
  IDLE = 0,
  LOOK_AT_PLR = 1,
  FOLLOW_PLR = 2,
  TARGETING = 3,
  RETREATING = 4
};


/* Erwin is a Companion Combatant of the "Maverick" archetype. Their
 * behavior is designed to make sure the player doesn't get overwhelmed.
 * Mainly by going out of their way to draw enemy aggro if too many are
 * targeting the player. As such, they tend to be a lot more 
 * self-sufficent than other companions.*/
class Erwin : public PartyMember {
public:
  Erwin(Companion *data, Mary *player);
  ~Erwin();

  void setEnabled(bool value) override;

  void behavior() override;

  /* This is considered the start point of the Erwin's behavior tree.
   * The function is meant to be ran while the companion is idle.*/
  void rootBehavior();

  /* Meant to be called while Erwin is targeting an enemy. The function
   * is designed to make the companion seem more "alive", by making it
   * stop or retreat at random intervals.*/
  void targetingBehavior();
  void chooseTarget();

  void decideAttack();
  void attackMP();
  void attackHP();

  void setGoal(ErwinGoals goal, float chance);

  void update() override;
  void neutralLogic();

  void lookAtPlayer();
  void followPlayer();
  void targetingLogic();
  void retreatingLogic();

  void wait(float time);
  void wait(float min, float max);
  void waitTimer();

  void movement();
  void animationLogic();

  void draw() override;
  void drawDebug() override;

  Animation *getIdleAnim();
  Rectangle *getStunSprite();

  static SpriteAtlas atlas;

  ErwinGoals ai_goal = ErwinGoals::IDLE;
  float tick_clock = 0.0;
private:
  const float default_speed = 68;
  int moving_x = 0;
  bool has_moved = false;

  Mary *player;
  float preferred_plr_distance = 128;
  float attack_distance = 24;
  float contest_distance = 96;

  float retreat_time = 0.5;
  float retreat_clock = 0.0;

  bool waiting = false;
  float wait_time = 0.20;
  float wait_clock = 0.0;

  Animation anim_idle = {{0, 1}, 2.0};
  Animation anim_crit = {{2, 3}, 1.0};

  float anim_move_speed = 0.2;
  Animation anim_move = {{4, 5, 6, 5}, anim_move_speed};
  Animation anim_dead = {{7, 9}, 0.25};

  AtkAnimSet atk_mp_set = {
    {{10, 11}, 0.05},
    {{11, 10}, 0.05},
    12
  };

  AtkAnimSet atk_hp_set = {
    {{13}, 0.0},
    {{15, 16}, 0.0},
    14
  };
};
