#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/party_member.h"
#include "base/status_effect.h"
#include "combat/status_effects/endurance.h"
#include <plog/Log.h>


Endurance::Endurance(PartyMember *afflicted): 
  StatusEffect(StatusID::ENDURANCE, StatusType::POSITIVE, afflicted)
{
  name = "Endurance";
  persistent = false;
  this->afflicted = afflicted;

  tenacity_given = afflicted->max_life * 0.20;
  PLOGI << "Tenacity to be given: " << tenacity_given;
  PLOGI << "Resilience to be gained: " << resilience_gained;
}

Endurance::~Endurance() {
  if (end) {
    PLOGD << "Reversing stat boosts.";
    afflicted->resilience -= resilience_gained;
    afflicted->sfx.play("endurance_lost");
  }
}

void Endurance::refresh() {
  float magnitude = afflicted->max_life * 0.20;
  float max_tenacity = afflicted->max_life * afflicted->tp_threshold;

  afflicted->increaseTenacity(magnitude, 0.50);
  tenacity_given = Clamp(tenacity_given + magnitude, 0, max_tenacity);

  PLOGI << "Refreshed status effect.";
  clock = 0.0;
  afflicted->sfx.play("endurance_gain");
}

void Endurance::init(bool hide_text) {
  StatusEffect::init(hide_text);

  PLOGI << "Applying stat boosts.";
  afflicted->increaseTenacity(tenacity_given, 0.5);
  afflicted->resilience += resilience_gained;

  PLOGD << "Result: {TP: " << afflicted->tenacity << ", RES: " <<
    afflicted->resilience << "}";
  afflicted->sfx.play("endurance_gain");
}

void Endurance::logic() {
  if (clock < 1.0) {
    clock += Game::deltaTime() / delay;
    return;
  }

  float max_life = afflicted->max_life;
  float magnitude = (afflicted->max_life * 0.005) * Game::deltaTime();

  if (magnitude > tenacity_given) {
    magnitude = tenacity_given;
  }

  afflicted->damageTenacity(magnitude);
  tenacity_given -= magnitude;

  float tenacity = afflicted->tenacity;
  if (tenacity_given == 0 || tenacity == 0) {
    PLOGI << "The effect has worn off.";
    end = true;
  }
}
