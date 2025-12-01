#pragma once
#include <raylib.h>
#include "base/combatant.h"
#include "base/party_member.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"


class EnemyHud {
public:
  EnemyHud(Vector2 position);
  ~EnemyHud();
  void assign(Mary *&player, PartyMember *&companion);

  void update();
  void draw();
  void drawLifeGauge(Combatant *target);

  static SpriteAtlas atlas;
private:
  Mary **player;
  PartyMember **companion;

  Vector2 main_position;
  Texture life_bar;
  Texture life_empty;
};
