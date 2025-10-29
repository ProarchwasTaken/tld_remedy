#include <memory>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/status_effect.h"
#include "base/party_member.h"
#include "data/combatant_event.h"
#include "combat/status_effects/mending.h"
#include <plog/Log.h>

using std::unique_ptr;


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

void Mending::evaluateEvent(unique_ptr<CombatantEvent> &event) {
  if (event->sender != afflicted) {
    return;
  }

  if (event->event_type != CombatantEVT::TOOK_DAMAGE) {
    return;
  }

  TookDamageCBT *dmg_event = static_cast<TookDamageCBT*>(event.get());
  
  if (dmg_event->damage_type == DamageType::LIFE) {
    PLOGI << "Mending status effect has been interrupted.";
    end = true;
  }
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
