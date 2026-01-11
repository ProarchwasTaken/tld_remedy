#pragma once
#include "base/entity.h"
#include "data/entity.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"


class SavePoint : public Entity {
public:
  SavePoint(SavePointData &data);
  ~SavePoint();

  void update() override;
  void draw() override;

  static SpriteAtlas atlas;
private:
  Animation anim_idle = {{0, 1, 2, 3}, 0.20};
};
