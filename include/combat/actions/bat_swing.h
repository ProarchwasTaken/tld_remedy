#pragma once
#include "base/combat_action.h"
#include "data/damage.h"
#include "data/rect_ex.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"


class BatSwing : public CombatAction {
public:
  BatSwing(Mary *user);
  ~BatSwing();

  void updateHitboxOffset();

  void windUp() override;
  void freeTurning();

  void action() override;
  void endLag() override;

  void drawDebug() override;
private:
  Mary *user;
  RectEx hitbox;
  DamageData data;

  SpriteAtlas *atlas;
  Animation anim_windup = {{38, 39, 40}, 0.2};
  Animation anim_end = {{42, 43}, 0.3};
};
