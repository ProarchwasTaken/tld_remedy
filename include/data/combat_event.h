#pragma once
#include <string>
#include <raylib.h>
#include "base/combatant.h"
#include "enums.h"
#include "system/sprite_atlas.h"


struct CombatEvent {
  CombatEVT event_type;
};

struct DeleteEntityCB : CombatEvent {
  int entity_id;
};

struct CreateDmgNumCB : CombatEvent {
  Combatant *target;
  DamageType damage_type;
  float damage_taken;
};

struct CreateStatTxtCB : CombatEvent {
  Combatant *target;
  std::string text;
  Color color;
};

struct CreateAfterImgCB : CombatEvent {
  SpriteAtlas *atlas;
  Rectangle *sprite;
  Vector2 position;
  float life_time = 0.25;
  Color tint = WHITE;
};
