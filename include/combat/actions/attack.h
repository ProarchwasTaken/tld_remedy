#pragma once
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/damage.h"
#include "data/animation.h"
#include "data/rect_ex.h"
#include "system/sprite_atlas.h"


struct AtkAnimSet {
  Animation wind_up;
  Animation end_lag;
  int atk_sprite = 0;
};


/* The Attack CombatAction is a very versatile Action that's designed to
 * be used by any Combatant as long as the right arguments are provided
 * on initialization. It's the most simple way to allow the Combatant to 
 * inflict damage of any kind.*/
class Attack : public CombatAction {
public:
  Attack(Combatant *user, SpriteAtlas &user_atlas, 
         RectEx hitbox, AtkAnimSet &anim_set);
  void updateAnimFrameDuration();

  void windUp() override;
  void action() override;
  void endLag() override;

  void drawDebug() override;
private:
  RectEx hitbox;

  DamageData data;
  AtkAnimSet *anim_set;
  SpriteAtlas *user_atlas;
  bool attack_connected = false;
};
