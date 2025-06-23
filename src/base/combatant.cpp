#include <cassert>
#include <cstddef>
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <plog/Log.h>
#include "enums.h"
#include "game.h"
#include "data/damage.h"
#include "base/combat_action.h"
#include "base/combatant.h"

using std::string, std::unique_ptr;


Combatant *Combatant::getCombatantByID(int entity_id) {
  Combatant *result = NULL;

  for (Combatant *combatant : existing_combatants) {
    if (combatant->entity_id == entity_id) {
      result = combatant;
      break;
    }
  }

  return result;
}

Combatant::Combatant(string name, CombatantTeam team, Vector2 position, 
                     Direction direction)
{
  this->name = name;
  this->position = position;
  this->direction = direction;

  entity_type = EntityType::COMBATANT;
  this->team = team;
 
  bool successful = existing_combatants.emplace(this).second;
  assert(successful);

  state = CombatantState::NEUTRAL;
  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "]";
}

Combatant::~Combatant() {
  int erased = existing_combatants.erase(this);
  assert(erased == 1);

  action.reset();
  PLOGI << "Removed combatant: '" << name << "'";
}

void Combatant::takeDamage(DamageData &data) {
  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "] has "
  << "taken damage from: '" << data.assailant->name << "' [ID: " <<
  data.assailant->entity_id << "]";
  if (state == CombatantState::ACTION) {
    action->intercept(data);
  }

  float damage = damageCalulation(data);
  PLOGI << "Result: " << damage;

  if (data.damage_type == DamageType::LIFE) {
    PLOGD << "Directing damage towards Combatant's Life.";
    damageLife(damage);
  }
  else {
    PLOGD << "Directing damage towards Combatant's Morale.";
    damageMorale(damage);
    data.assailant->increaseMorale(damage);
  }

  if (data.stun_time != 0) {
    enterHitstun(data);
  }
}

float Combatant::damageCalulation(DamageData &data) {
  PLOGI << "Now performing damage calulation.";
  assert(data.assailant != NULL);
  Combatant *assailant = data.assailant;

  bool atk_not_set = data.a_atk == NULL;
  bool def_not_set = data.b_def == NULL;

  switch (data.calulation) {
    case DamageType::LIFE: {
      if (atk_not_set) {
        data.a_atk = &assailant->offense;
      }

      if (def_not_set) {
        data.b_def = &defense;
      }
    }
    case DamageType::MORALE: {
      if (atk_not_set) {
        data.a_atk = &assailant->intimid;
      }

      if (def_not_set) {
        data.b_def = &persist;
      }
    }
  }

  float base_damage = data.base_damage;
  int a_atk = *data.a_atk;
  int b_def = *data.b_def;

  PLOGD << "Base Damage: " << base_damage;
  PLOGD << "Assailant ATK: " << a_atk << " vs. Victim DEF: " << b_def;
  return (base_damage + a_atk) - b_def;
}

void Combatant::damageLife(float magnitude) {
  life = life - magnitude;
  PLOGI << "Life decreased to: " << life;
}

void Combatant::damageMorale(float magnitude) {
  PLOGD << "Combatant does not possess Morale to damage.";
}

void Combatant::increaseMorale(float magnitude) {
  PLOGD << "Combatant does not possess Morale to increase.";
}

void Combatant::enterHitstun(DamageData &data) {
  StunType stun_type = data.stun_type;

  float multiplier;
  switch (stun_type) {
    case StunType::NORMAL: {
      multiplier = 1.0;
      break;
    }
    case StunType::DEFENSIVE: {
      multiplier = 0.80;
      break;
    }
    case StunType::STAGGER: {
      multiplier = 1.20;
    }
  }

  stun_time = data.stun_time * multiplier;
  knockback = data.knockback;
  kb_direction = data.assailant->direction;

  state = CombatantState::HIT_STUN;
  if (action != nullptr) {
    cancelAction();
  }

  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "]"
  " has entered hitstun for: " << stun_time << " seconds.";
}

void Combatant::stunLogic() {
  assert(stun_time != 0 && "enterHitstun has to be called first!");
  stun_clock += Game::time() / stun_time;
  stun_clock = Clamp(stun_clock, 0.0, 1.0);

  if (knockback != 0) {
    applyKnockback();
  }

  if (stun_clock == 1.0) {
    exitHitstun();
  }
}

void Combatant::applyKnockback() {
  float percentage = 1.0 - stun_clock;
  position.x += (knockback * percentage) * kb_direction;

  rectExCorrection(bounding_box, hurtbox);
}

void Combatant::exitHitstun() {
  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "]"
  " has exited hit stun.";
  state = CombatantState::NEUTRAL;
  stun_clock = 0.0;
  stun_time = 0;
  knockback = 0;
}

void Combatant::performAction(unique_ptr<CombatAction> &action) {
  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "]" <<
    " is performing ACTION: '" << action->name << "'";
  if (this->action != nullptr) {
    this->action.reset();
  }

  this->action.swap(action);
  state = CombatantState::ACTION;
} 

void Combatant::cancelAction() {
  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "] action"
  << " has been canceled!";
  action.reset();

  if (state != CombatantState::HIT_STUN) {
    state = CombatantState::NEUTRAL;
  }
}


void Combatant::drawDebug() {
  Entity::drawDebug();
  DrawRectangleLinesEx(hurtbox.rect, 1, RED);
}
