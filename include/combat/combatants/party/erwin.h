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
  RETREATING = 4,
};


class Erwin : public PartyMember {
public:
  Erwin(Companion *data, Mary *player);
  ~Erwin();

  void setEnabled(bool value) override;

  void behavior() override;
  void rootBehavior();
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
  float preferred_distance = 24;

  float retreat_time = 0.5;
  float retreat_clock = 0.0;

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
