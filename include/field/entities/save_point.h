#pragma once
#include <raylib.h>
#include "base/entity.h"
#include "data/entity.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "field/actors/player.h"


class SavePoint : public Entity {
public:
  SavePoint(SavePointData &data);
  ~SavePoint();

  void interact();

  void update() override;
  void draw() override;

  static SpriteAtlas atlas;
private:
  PlayerActor *plr;
  bool in_range = false;

  bool rest_point = false;
  Rectangle *alt_sprite;

  Animation anim_idle = {{0, 1, 2, 3}, 0.20};
};
