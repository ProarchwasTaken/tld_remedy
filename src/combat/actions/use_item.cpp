#include <cassert>
#include <memory>
#include <cstddef>
#include "enums.h"
#include "game.h"
#include "base/combat_action.h"
#include "base/status_effect.h"
#include "base/party_member.h"
#include "data/combat_event.h"
#include "utils/animation.h"
#include "combat/system/evt_handler.h"
#include "combat/status_effects/mending.h"
#include "combat/status_effects/refreshed.h"
#include "combat/status_effects/endurance.h"
#include "combat/status_effects/despondent.h"
#include "combat/combatants/party/mary.h"
#include "combat/actions/use_item.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique;


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

void UseItem::applyItemEffect() {
  assert(target != NULL);

  switch (item) {
    case ItemID::I_BANDAGE: {
      PLOGI << "Applying effect of item: Improvised Bandage.";
      applyMending(0.25, 0.05);
      break;
    }
    case ItemID::M_SPLINT: {
      PLOGI << "Applying effect of item: Makeshift Splint.";
      applySplint();
      break;
    }
    case ItemID::S_BANDAGE: {
      PLOGI << "Applying effect of item: Sterilized Bandage.";
      applyMending(0.50, 0.10);
      break;
    }
    case ItemID::S_WATER: {
      PLOGI << "Applying effect of item: Sparkling Water.";
      applyRefreshed();
      break;
    }
    case ItemID::P_KILLERS: {
      PLOGI << "Applying effect of item: Painkillers.";
      applyEndurance();
      break;
    }
    default: {
      PLOGE << "Invalid Item!!";
    }
  }
}

void UseItem::applyMending(float percentage, float speed) {
  if (target->life == target->max_life) {
    PLOGI << "Target is already at full Life!";
    user->sfx.play("mending_loss");
    return;
  }

  for (auto &effect : target->status) {
    if (effect->id == StatusID::MENDING) {
      Mending *mending = static_cast<Mending*>(effect.get());
      mending->refresh(percentage, speed);
      return;
    }
  }

  unique_ptr<StatusEffect> effect = make_unique<Mending>(target, 
                                                         percentage,
                                                         speed);
  target->afflictStatus(effect);
}

void UseItem::applySplint() {
  Despondent *despondent = NULL;
  bool cured = false;

  for (auto &effect : target->status) {
    if (effect->id == StatusID::DESPONDENT) {
      despondent = static_cast<Despondent*>(effect.get());
      continue;
    }

    if (!cured && effect->isPersistent()) {
      PLOGI << "Cured Statis Ailment: '" << effect->name << "'";
      effect->end = true;
      cured = true;
    }
  }

  user->sfx.play("item_splint");

  if (!cured || despondent == NULL) {
    return;
  }

  PLOGI << "Cured Despondence as well.";
  despondent->end = true;
}

void UseItem::applyRefreshed() {
  for (auto &effect : target->status) {
    if (effect->id == StatusID::REFRESHED) {
      Refreshed *refreshed = static_cast<Refreshed*>(effect.get());
      refreshed->refresh();
      return;
    }
  }

  unique_ptr<StatusEffect> effect = make_unique<Refreshed>(target);
  target->afflictStatus(effect);
}

void UseItem::applyEndurance() {
  for (auto &effect : target->status) {
    if (effect->id == StatusID::ENDURANCE) {
      Endurance *endurance = static_cast<Endurance*>(effect.get());
      endurance->refresh();
      return;
    }
  }

  unique_ptr<StatusEffect> effect = make_unique<Endurance>(target);
  target->afflictStatus(effect);
}

void UseItem::windUp() {
  SpriteAnimation::play(user->animation, &anim_windup, true);

  interval_clock += Game::deltaTime() / interval;
  if (interval_clock >= 1.0) {
    user->sfx.play("item_windup");
    interval_clock = 0.0;
  }

  if (target->state == DEAD) {
    phase = ActionPhase::END_LAG;
    proceed();
    return;
  }

  bool end_phase = state_clock == 1.0;
  if (end_phase) {
    user->sprite = &atlas->sprites[33];
    user->sfx.play("item_use");
  }
  else {
    user->sprite = &atlas->sprites[*user->animation->current]; 
  }
}

void UseItem::action() {
  SpriteAnimation::play(user->animation, &anim_use, false);
  user->sprite = &atlas->sprites[*user->animation->current];

  if (target->state == DEAD) {
    phase = ActionPhase::END_LAG;
    proceed();
    return;
  }

  bool end_phase = state_clock == 1.0;
  if (end_phase) {
    applyItemEffect();
    CombatHandler::raise<RemoveItemCB>(CombatEVT::REMOVE_ITEM, item);
  }
}
