#pragma once
#include <memory>
#include <raylib.h>
#include "base/combatant.h"
#include "base/party_member.h"
#include "data/session.h"
#include "data/animation.h"
#include "data/ai_behavior.h"
#include "data/combatant_event.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"
#include "combat/actions/attack.h"
#include "combat/actions/ghost_step.h"
#include "combat/actions/evade.h"


enum class ErwinGoals {
  IDLE = 0,
  LOOK_AT_PLR = 1,
  FOLLOW_PLR = 2,
  TARGETING = 3,
  RETREATING = 4,
  DODGING = 5
};


/* The parameters that govern how Erwin will behave. Depending on the
 * gamestate, these variables may change.*/
struct ErwinAI : AIBehavior {
  ErwinAI() {
    contesting = {0.40, 0.10, 0.40, 0.40, 0.10, 0.25};
    targeting = {0.25, 0.10, 0.75};
    retreating = {0.50, 0.10, 0.50};
    dodging = {0.80, 0.25, 0.50, 0.50, 1.0, 0.50, 0.80};

    retaliation_chance = 0.75;
  }
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
  void evaluateEvent(std::unique_ptr<CombatantEvent> &event) override;
  void warningHandling(WarningCBT *event);
  void damageHandling(TookDamageCBT *event);
  void retaliation(Combatant *assailant, float chance);
  float chanceCalculation(WarningCBT *event, bool from_target,
                          bool in_range);

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
  void ghoststep(int direction_x);
  void evade();

  void setGoal(ErwinGoals goal, float chance);

  void update() override;
  void neutralLogic();

  void lookAtPlayer();
  void followPlayer();
  void targetingLogic();
  void retreatingLogic();
  void dodgingLogic();

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
  std::unique_ptr<AIBehavior> ai_behavior;
  
  float tick_clock = 0.0;
private:
  const float default_speed = 68;
  int moving_x = 0;
  bool has_moved = false;

  Mary *player;
  float preferred_plr_distance = 128;
  float attack_distance = 30;
  float contest_distance = 96;

  float retreat_time = 0.5;
  float retreat_clock = 0.0;

  float dodge_time = 0.0;
  float dodge_clock = 0.0;

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

  GSSpriteSet gs_set = {17, 18, 8};
  EvadeSpriteSet ev_set = {19, 20, 19, 21, 0};
};
