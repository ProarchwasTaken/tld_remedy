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
  persistent = false;
  this->afflicted = afflicted;

  to_be_healed = afflicted->max_life * percentage;
  PLOGI << "Life to be healed: " << to_be_healed;

  this->speed = speed;
}

Mending::~Mending() {
  if (end) {
    afflicted->sfx.play("mending_loss");
  }
}

void Mending::refresh(float percentage, float speed) {
  float new_value = afflicted->max_life * percentage;
  if (new_value > to_be_healed) {
    to_be_healed = new_value;
  }

  if (this->speed < speed) {
    this->speed = speed;
  }

  PLOGI << "Refreshed status effect.";
  afflicted->sfx.play("mending_gain");
}

void Mending::init(bool hide_text) {
  StatusEffect::init(hide_text);
  afflicted->sfx.play("mending_gain");
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
  float max_life = afflicted->max_life;
  float recovery = afflicted->recovery;

  float magnitude = (max_life * speed) * recovery;
  magnitude = magnitude * Game::deltaTime();

  if (magnitude > to_be_healed) {
    magnitude = to_be_healed;
  } 

  afflicted->increaseLife(magnitude);
  to_be_healed -= magnitude;

  float life = afflicted->life;
  if (to_be_healed <= 0 || life == max_life) {
    PLOGI << "Healing complete.";
    end = true;
  }
}
