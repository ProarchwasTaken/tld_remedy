#pragma once
#include <raylib.h>
#include "enums.h"
#include "system/sprite_atlas.h"


class Portrait {
public:
  Portrait(Vector2 position);
  ~Portrait();
  void update(float percentage);
  void draw(PartyMemberID member_id);

  float fade_clock = 0.0;
private:
  static SpriteAtlas atlas;
  Vector2 position;

  float portrait_height = 0;
  float fade_time = 0.25;
};
