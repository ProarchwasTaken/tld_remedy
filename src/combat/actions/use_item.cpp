#include <cstddef>
#include "enums.h"
#include "base/combat_action.h"
#include "base/party_member.h"
#include "utils/animation.h"
#include "combat/combatants/party/mary.h"
#include "combat/actions/use_item.h"


UseItem::UseItem(Mary *user, PartyMember *target, ItemID item, 
                 float wind_up):
CombatAction(ActionID::USE_ITEM, ActionType::SUPPORT, user, wind_up, 0.5, 
             0.0) 
{
  name = "Use Item";
  atlas = &Mary::atlas;

  this->user = user;
  this->target = target;
  this->item = item;
}

UseItem::~UseItem() {
  user->animation = NULL;
}

void UseItem::windUp() {
  SpriteAnimation::play(user->animation, &anim_windup, true);

  bool end_phase = state_clock == 1.0;
  if (end_phase) {
    user->sprite = &atlas->sprites[33];
  }
  else {
    user->sprite = &atlas->sprites[*user->animation->current]; 
  }
}

void UseItem::action() {
  SpriteAnimation::play(user->animation, &anim_use, false);
  user->sprite = &atlas->sprites[*user->animation->current];
}
