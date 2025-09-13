#pragma once
#include <raylib.h>
#include "base/party_member.h"
#include "data/session.h"
#include "system/sprite_atlas.h"


class Erwin : public PartyMember {
public:
  Erwin(Companion *com);
  ~Erwin();

  void behavior() override;

  void update() override;

  void draw() override;
  void drawDebug() override;

  static SpriteAtlas atlas;
};
