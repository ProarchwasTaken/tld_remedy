#pragma once
#include <raylib.h>
#include "enums.h"
#include "base/entity.h"
#include "data/combat_event.h"
#include "system/sprite_atlas.h"


class AfterImage : public Entity {
public:
  AfterImage(CreateAfterImgCB *data);
  ~AfterImage();

  void update() override;
  void draw() override;
private:
  SpriteAtlas *atlas;
  Color tint;

  Direction direction;

  float life_time;
  float life_clock = 0.0;
};
