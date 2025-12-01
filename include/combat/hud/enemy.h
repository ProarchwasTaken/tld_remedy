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
  void drawTargetHud(Combatant *target, Vector2 position);
  void drawGauge(Combatant *target, Vector2 position, float width, 
                 float max_width, bool overflow);

  static SpriteAtlas atlas;
private:
  Mary **player;
  PartyMember **companion;

  Vector2 main_position;
  Texture life_bar;
  Texture life_empty;

  static constexpr float LIFE_PER_SEGMENT = 20;
};
