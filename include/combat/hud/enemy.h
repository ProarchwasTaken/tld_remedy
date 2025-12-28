#pragma once
#include <cstddef>
#include <array>
#include <memory>
#include <raylib.h>
#include "base/combatant.h"
#include "base/party_member.h"
#include "data/combatant_event.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"


struct TargetData {
  Combatant *target = NULL;
  float prev_life = 0.0;
  float red_clock = 0.0;
  float end_clock = 0.0;
};


class EnemyHud {
public:
  EnemyHud(Vector2 position);
  ~EnemyHud();
  void assign(Mary *&player, PartyMember *&companion);
  void evaluateEvent(std::unique_ptr<CombatantEvent> &event);
  void damageHandling(Combatant *sender);

  void update();
  void targetCheck(PartyMember *member);
  void targetLogic();
  void redGuageTimer(TargetData &data);

  void draw();
  void drawTargetHud(TargetData &data, Vector2 position);

  void drawLifeGauge(TargetData &data, Vector2 position, float width, 
                     float max_width, bool overflow);
  void drawGaugeNormal(TargetData &data, Vector2 position, float width);
  void drawGaugeOverflow(TargetData &data, Vector2 position, float width,
                         float max_width);

  void drawName(Combatant *target, Vector2 position);

  void drawSegments(TargetData &data, Vector2 position);
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
  static constexpr float RED_GAUGE_TIME = 0.10;
  static constexpr float END_TIME = 0.5;
};
