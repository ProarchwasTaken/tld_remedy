#pragma once
#include <raylib.h>
#include "enums.h"
#include "system/sprite_atlas.h"


class Portrait {
public:
  Portrait(Vector2 position);
  ~Portrait();
  void draw(PartyMemberID member_id);
private:
  static SpriteAtlas atlas;
  Vector2 position;
};
