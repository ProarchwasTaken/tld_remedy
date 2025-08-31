#pragma once
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "data/damage.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"


class KnifePiercer : public CombatAction {
public:
  KnifePiercer(Mary *user);
  ~KnifePiercer();

  void windUp() override;
  void action() override;
  void endLag() override;

  void drawDebug() override;
private:
  float velocity = 90;

  RectEx hitbox;
  DamageData data;

  SpriteAtlas *atlas;
  Animation anim_windup = {{24, 25}, 0.15};
  Animation anim_end = {{27, 28}, 0.10};
  Animation anim_heave = {{29, 30, 24}, 0.10};
};
