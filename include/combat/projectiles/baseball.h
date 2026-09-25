#pragma once
#include <cstddef>
#include <set>
#include <utility>
#include <raylib.h>
#include "base/combatant.h"
#include "base/projectile.h"
#include "data/damage.h"
#include "system/sound_atlas.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/xander.h"


class Baseball : public Projectile {
public:
  Baseball(Combatant *owner, Vector2 position);
  bool isOwnerMary();
  void findXander();

  void update() override;
  void swingDetection();

  bool checkMary();
  bool checkXander();

  void swingSuccessful();
  void whipSuccessful();

  void criticalHit();
  void critEffect();
  void critEnd();

  void afterimages();
  void hitRegistration(std::set<std::pair<float, Combatant*>> &hits);
  void inflictDamage(std::set<std::pair<float, Combatant*>> &hits);
  void onEndLife() override;

  void draw() override;

  static SpriteAtlas atlas;
private:
  DamageData data;
  int atk;

  float distance_traveled = 0;

  std::set<std::pair<float, Combatant*>> hits;
  bool hit_by_swing = false;
  bool use_crit_effect = false;
  bool end_crit_effect = false;

  SoundAtlas *sfx;
  Xander *xander = NULL;
};
