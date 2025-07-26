#pragma once
#include "base/combatant.h"
#include "base/combat_action.h"
#include "system/sprite_atlas.h"


struct GSSpriteSet {
  int forward_sprite = 0;
  int back_sprite = 0;
  int end_sprite = 0;
};

/* GhostStep is an CombatAction that's designed to serve as a quick and
 * reliable defensive option for all Combatants. While it's main
 * functionality is to make the Combantant intangible to hitboxes during
 * it's active phase, it's versatile as it could also be used for other
 * purposes like extending combos, or as a spacing tool.*/
class GhostStep : public CombatAction {
public:
  GhostStep(Combatant *user, SpriteAtlas &user_atlas, int direction_x,
            GSSpriteSet &sprite_set);

  void windUp() override {}
  void action() override;
  void endLag() override;
private:
  float velocity; 

  GSSpriteSet *sprite_set;
  SpriteAtlas *user_atlas;
};
