#include <raylib.h>
#include <raymath.h>
#include <memory>
#include "enums.h"
#include "game.h"
#include "base/party_member.h"
#include "base/status_effect.h"
#include "data/combatant_event.h"
#include "combat/status_effects/broken_arm.h"
#include "combat/status_effects/crippled_leg.h"
#include "combat/status_effects/mangled.h"
#include "combat/status_effects/endurance.h"
#include <plog/Log.h>

using std::unique_ptr;


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
    PLOGI << "Reversing stat boosts.";
    afflicted->resilience -= resilience_gained;

    PLOGI << "Attempting to re-enable all persistent status ailments.";
    for (auto &effect : afflicted->status) {
      if (effect == nullptr) {
        continue;
      }

      bool negative = effect->type == StatusType::NEGATIVE;
      if (negative && effect->isPersistent()) {
        enableAilment(effect.get());
      }
    }

    afflicted->sfx.play("endurance_lost");
  }
}

void Endurance::refresh() {
  float magnitude = afflicted->max_life * 0.20;
  float max_tenacity = afflicted->max_life * afflicted->tp_threshold;
  float limit = max_tenacity - afflicted->tenacity;

  afflicted->increaseTenacity(magnitude, 0.50);
  tenacity_given = Clamp(tenacity_given + magnitude, 0, limit);

  PLOGI << "Refreshed status effect.";
  clock = 0.0;
  afflicted->sfx.play("endurance_gain");
}

void Endurance::init(bool hide_text) {
  StatusEffect::init(hide_text);

  PLOGI << "Applying stat boosts.";
  afflicted->increaseTenacity(tenacity_given, 0.5);
  afflicted->resilience += resilience_gained;

  PLOGI << "Attempting to negate all persistent status ailments.";
  for (auto &effect : afflicted->status) {
    bool negative = effect->type == StatusType::NEGATIVE;
    if (negative && effect->isPersistent()) {
      negateAilment(effect.get());
    }
  }

  PLOGD << "Result: {TP: " << afflicted->tenacity << ", RES: " <<
    afflicted->resilience << "}";
  afflicted->sfx.play("endurance_gain");
}

void Endurance::evaluateEvent(unique_ptr<CombatantEvent> &event) {
  if (event->sender != afflicted) {
    return;
  }

  if (event->event_type != CombatantEVT::EFFECT_GAINED) {
    return;
  }

  auto *effect_event = static_cast<EffectGainedCBT*>(event.get());
  StatusID effect_id = effect_event->effect_id;

  for (auto &effect : afflicted->status) {
    if (effect->id != effect_id) {
      continue;
    }

    bool negative = effect->type == StatusType::NEGATIVE;
    if (negative && effect->isPersistent()) {
      PLOGI << "Acknowledging EffectGained event sent by afflicted.";
      negateAilment(effect.get());
    }
  }
}

void Endurance::negateAilment(StatusEffect *effect) {
  switch (effect->id) {
    case StatusID::BROKEN_ARM: {
      BrokenArm *broken_arm = static_cast<BrokenArm*>(effect);
      if (!broken_arm->negated) {
        broken_arm->negateEffect();
      }

      break;
    }
    case StatusID::CRIPPLED_LEG: {
      CrippledLeg *crippled_leg = static_cast<CrippledLeg*>(effect);
      if (!crippled_leg->negated) {
        crippled_leg->negateEffect();
      }

      break;
    }
    case StatusID::MANGLED: {
      Mangled *mangled = static_cast<Mangled*>(effect);
      if (!mangled->negated) {
        mangled->negateEffect();
      }

      break;
    }
    default: {
      PLOGE << "Invalid status effect!!";
    }
  }
}

void Endurance::enableAilment(StatusEffect *effect) {
  switch (effect->id) {
    case StatusID::BROKEN_ARM: {
      BrokenArm *broken_arm = static_cast<BrokenArm*>(effect);
      if (broken_arm->negated) {
        broken_arm->applyPenalty();
      }

      break;
    }
    case StatusID::CRIPPLED_LEG: {
      CrippledLeg *crippled_leg = static_cast<CrippledLeg*>(effect);
      if (crippled_leg->negated) {
        crippled_leg->applyPenalty();
      }

      break;
    }
    case StatusID::MANGLED: {
      Mangled *mangled = static_cast<Mangled*>(effect);
      if (mangled->negated) {
        mangled->applyPenalty();
      }

      break;
    }
    default: {
      PLOGE << "Invalid status effect!!";
    }
  }

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
