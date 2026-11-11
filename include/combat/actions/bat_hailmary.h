#pragma once
#include "base/combat_action.h"
#include "data/animation.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "combat/combatants/party/mary.h"


class BatHailMary : public CombatAction {
public:
  BatHailMary(Mary *user);
  ~BatHailMary();

  void windUp() override;
  void createProjectile();

  void action() override;
  void endLag() override {}
private:
  SoundAtlas *sfx;

  SpriteAtlas *atlas;
  Animation anim_windup = {{45, 46, 47, 48}, 0.10};
};
