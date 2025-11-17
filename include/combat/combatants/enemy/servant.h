#pragma once
#include <raylib.h>
#include "enums.h"
#include "base/enemy.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "combat/actions/attack.h"


class Servant : public Enemy {
public:
  Servant(Vector2 position, Direction direction);
  ~Servant();

  void behavior() override;
  void attackMP();

  void update() override;

  Rectangle *getStunSprite();
  void draw() override;
  void drawDebug() override;

  static SpriteAtlas atlas;
private:
  Animation anim_dead = {{5, 6}, 0.5};

  AtkAnimSet atk_mp_set = {
    {{7, 8}, 0.05},
    {{10}, 0.05},
    9
  };
};
