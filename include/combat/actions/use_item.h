#pragma once
#include "enums.h"
#include "data/animation.h"
#include "base/combat_action.h"
#include "base/party_member.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"


class UseItem : public CombatAction {
public:
  UseItem(Mary *user, PartyMember *target, ItemID item, float wind_up);
  ~UseItem();

  void windUp() override;
  void action() override;
  void endLag() override {};
private:
  Mary *user;
  PartyMember *target;
  ItemID item;

  SpriteAtlas *atlas;
  Animation anim_windup = {{31, 32}, 0.25};
  Animation anim_use = {{33, 34, 35, 36, 37}, 0.10};
};

