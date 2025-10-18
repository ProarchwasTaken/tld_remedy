#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "base/status_effect.h"
#include "base/party_member.h"
#include "data/damage.h"
#include "combat/status_effects/mending.h"
#include <cstddef>
#include <plog/Log.h>
#include <raymath.h>


Mending::Mending(PartyMember *afflicted, float percentage, float speed): 
  StatusEffect(StatusID::MENDING, StatusType::POSITIVE, afflicted)
{
  name = "Mending";
  persistant = false;
  this->afflicted = afflicted;

  to_be_healed = afflicted->life * percentage;
  PLOGI << "Life to be healed: " << to_be_healed;

  this->speed = speed;
}

void Mending::intercept(DamageData &data) {
  if (data.damage_type != DamageType::LIFE) {
    return;
  }

  CombatAction *action = NULL;
  bool evaded_attack = false;
  if (afflicted->state == ACTION) {
    action = afflicted->action.get();

    bool using_evade = action->id == ActionID::EVADE;
    evaded_attack = using_evade && action->phase == ActionPhase::ACTIVE;
  }

  if (action != NULL && evaded_attack) {
    return;
  }

  PLOGI << "Mending status effect has been interrupted.";
  end = true;
}

void Mending::logic() {
  float recovery = afflicted->recovery;
  float magnitude = (afflicted->life * speed) * recovery;
  magnitude = magnitude * Game::deltaTime();

  if (magnitude > to_be_healed) {
    magnitude = to_be_healed;
  } 

  float *life = &afflicted->life;
  float max_life = afflicted->max_life;

  *life = Clamp(*life + magnitude, 0, max_life);
  to_be_healed -= magnitude;

  if (to_be_healed <= 0 || *life == max_life) {
    PLOGI << "Healing complete.";
    end = true;
  }
}
