#pragma once
#include <cstddef>
#include <array>
#include <raylib.h>
#include "base/combatant.h"
#include "base/party_member.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"


struct TargetData {
  Combatant *target = NULL;
  float clock = 0.0;
};


class EnemyHud {
public:
  EnemyHud(Vector2 position);
  ~EnemyHud();
  void assign(Mary *&player, PartyMember *&companion);

  void update();
  void targetCheck(PartyMember *member);
  void targetTimer();

  void draw();
  void drawTargetHud(Combatant *target, Vector2 position);
  void drawGauge(Combatant *target, Vector2 position, float width, 
                 float max_width, bool overflow);
  void drawName(Combatant *target, Vector2 position);

  void drawSegments(Combatant *target, Vector2 position);
  void drawTargetReticle(Combatant *target, Vector2 position, 
                         float frame_width);

  static SpriteAtlas atlas;
private:
  Mary **player;
  PartyMember **companion;
  std::array<TargetData, 3> targets;

  Vector2 main_position;
  Texture life_bar;
  Texture life_empty;

  static constexpr float LIFE_PER_SEGMENT = 20;
};
