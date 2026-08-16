#pragma once
#include <set>
#include <utility>
#include <raylib.h>
#include "base/combatant.h"
#include "base/projectile.h"
#include "data/damage.h"
#include "system/sound_atlas.h"
#include "system/sprite_atlas.h"


class Baseball : public Projectile {
public:
  Baseball(Combatant *owner, Vector2 position);

  void update() override;
  void swingDetection();
  void swingSuccessful();

  void afterimages();
  void hitRegistration(std::set<std::pair<float, Combatant*>> &hits);
  void inflictDamage(std::set<std::pair<float, Combatant*>> &hits);

  void draw() override;

  static SpriteAtlas atlas;
private:
  DamageData data;
  int atk;

  float distance_traveled = 0;

  std::set<std::pair<float, Combatant*>> hits;
  bool hit_by_swing = false;
  SoundAtlas *sfx;
};
