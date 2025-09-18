#pragma once
#include <raylib.h>
#include "base/party_member.h"
#include "data/session.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"

enum class ErwinGoals {
  IDLE = 0,
  LOOK_AT_PLR = 1,
};


class Erwin : public PartyMember {
public:
  Erwin(Companion *data, Mary *player);
  ~Erwin();

  void behavior() override;
  void setGoal(ErwinGoals ai_goal);

  void update() override;
  void neutralLogic();

  void goalLogic();
  void lookAtPlayer();

  void movement();
  void animationLogic();

  void draw() override;
  void drawDebug() override;

  Animation *getIdleAnim();
  Rectangle *getStunSprite();

  static SpriteAtlas atlas;
private:
  Mary *player;
  ErwinGoals ai_goal = ErwinGoals::IDLE;

  const float default_speed = 68;
  int moving_x = 0;
  bool has_moved = false;

  Animation anim_idle = {{0, 1}, 2.0};
  Animation anim_crit = {{2, 3}, 1.0};

  float anim_move_speed = 0.2;
  Animation anim_move = {{4, 5, 6, 5}, anim_move_speed};
};
