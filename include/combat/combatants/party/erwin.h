#pragma once
#include <raylib.h>
#include "base/party_member.h"
#include "data/session.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"


class Erwin : public PartyMember {
public:
  Erwin(Companion *com);
  ~Erwin();

  void behavior() override;

  void update() override;
  void neutralLogic();
  void movement();

  void draw() override;
  void drawDebug() override;

  Animation *getIdleAnim();
  Rectangle *getStunSprite();

  static SpriteAtlas atlas;
private:
  const float default_speed = 68;
  int moving_x = 0;
  bool has_moved = false;

  Animation anim_idle = {{0, 1}, 2.0};
  Animation anim_crit = {{2, 3}, 1.0};

  float anim_move_speed = 0.2;
  Animation anim_move = {{4, 5, 6, 5}, anim_move_speed};
};
