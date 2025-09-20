#include <cassert>
#include <cstddef>
#include <cmath>
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <plog/Log.h>
#include "enums.h"
#include "game.h"
#include "data/damage.h"
#include "data/combat_event.h"
#include "data/combatant_event.h"
#include "system/sound_atlas.h"
#include "utils/collision.h"
#include "base/combat_action.h"
#include "base/status_effect.h"
#include "combat/system/evt_handler.h"
#include "combat/system/cbt_handler.h"
#include "base/combatant.h"

using std::string, std::unique_ptr;
SoundAtlas Combatant::sfx("combat");

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
  sfx.use();
  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "]";
}

Combatant::~Combatant() {
  int erased = existing_combatants.erase(this);
  assert(erased == 1);

  action.reset();

  for (auto &status_effect : status) {
    status_effect.reset();
  }
  status.clear();

  sfx.release();
  PLOGI << "Removed combatant: '" << name << "'";
}

float Combatant::distanceTo(Combatant *combatant) {
  assert(combatant != NULL);
  assert(combatant != this);

  Vector2 difference = Vector2Subtract(position, combatant->position);
  float distance = Vector2Length(difference);
  return distance;
}

void Combatant::takeDamage(DamageData &data) {
  assert(state != CombatantState::DEAD);
  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "] has "
  << "taken damage from: '" << data.assailant->name << "' [ID: " <<
  data.assailant->entity_id << "]";

  if (!status.empty()) {
    for (auto &status_effect : status) {
      status_effect->intercept(data);
    }
  }

  if (state == CombatantState::ACTION) {
    action->intercept(data);
  }

  if (data.intercepted) {
    PLOGD << "Procedure has been intercepted. Canceling function.";
    return;
  }

  float damage = Clamp(damageCalulation(data), 0, 9999);
  PLOGD << "Damage Calculation: " << damage << ", Victim Life: " << life;
  finalIntercept(damage, data);
  applyDamage(damage, data);

  knockback = data.knockback;
  kb_direction = data.assailant->direction;
  kb_time = data.stun_time;
  kb_clock = 0.0;

  if (state != DEAD && data.stun_time != 0) {
    enterHitstun(data);
  }

  PLOGD << "Proceeding to queue TookDamage event.";
  CombatantHandler::queue<TookDamageCBT>(this, CombatantEVT::TOOK_DAMAGE,
                                         damage, data.damage_type, state,
                                         data.stun_time, data.stun_type,
                                         data.assailant);
}

float Combatant::damageCalulation(DamageData &data) {
  PLOGI << "Now performing damage calulation.";
  assert(data.assailant != NULL);
  Combatant *assailant = data.assailant;

  bool atk_not_set = data.a_atk == NULL;
  bool def_not_set = data.b_def == NULL;

  switch (data.calculation) {
    case DamageType::LIFE: {
      if (atk_not_set) data.a_atk = &assailant->offense;
      if (def_not_set) data.b_def = &defense;
      break;
    }
    case DamageType::MORALE: {
      if (atk_not_set) data.a_atk = &assailant->intimid;
      if (def_not_set) data.b_def = &persist; 
      break;
    }
  }

  float base_damage = data.base_damage;
  int a_atk = *data.a_atk;
  float atk_mod = data.atk_mod;

  int b_def = *data.b_def;
  float def_mod = data.def_mod;

  PLOGD << "Base Damage: " << base_damage;
  PLOGD << "Assailant ATK: " << a_atk << " vs. Victim DEF: " << b_def;
  return (base_damage + (a_atk * atk_mod)) - (b_def * def_mod);
}

void Combatant::applyDamage(float damage, DamageData &data) {
  damage_type = data.damage_type;

  if (damage_type == DamageType::LIFE) {
    PLOGD << "Directing damage towards Combatant's Life.";
    damageLife(damage);
    sfx.play("damage_hp");
  }
  else {
    PLOGD << "Directing damage towards Combatant's Morale.";
    damageMorale(damage);
    data.assailant->increaseMorale(damage);
    sfx.play("damage_mp");
  }

  if (damage != 0) {
    PLOGD << "Sending a request for a DamageNumber to be created.";
    CombatHandler::raise<CreateDmgNumCB>(CombatEVT::CREATE_DMG_NUM,
                                         this, data.damage_type, damage);
  }
}

void Combatant::damageLife(float magnitude) {
  life = life - magnitude;
  PLOGI << "Life decreased to: " << life;

  if (!critical_life && life < max_life * LOW_LIFE_THRESHOLD) {
    PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "] has"
    << " entered Critical Life!";
    critical_life = true;
  }

  if (life <= 0) {
    death();
  }
}

void Combatant::damageMorale(float magnitude) {
  PLOGD << "Combatant does not possess Morale to damage.";
}

void Combatant::increaseMorale(float magnitude) {
  PLOGD << "Combatant does not possess Morale to increase.";
}

void Combatant::enterHitstun(DamageData &data) {
  assert(state != CombatantState::DEAD);
  stun_type = data.stun_type;

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
      multiplier = 1.50;
    }
  }

  stun_time = data.stun_time * multiplier;
  stun_clock = 0.0;

  data.hit_stop *= multiplier;
  state = CombatantState::HIT_STUN;

  if (data.damage_type == DamageType::LIFE) {
    start_tint = Game::palette[32];
  }
  else {
    start_tint = Game::palette[40];
  }
  tint = start_tint;

  if (action != nullptr) {
    cancelAction();
  }

  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "]"
  " has entered hitstun for: " << stun_time << " seconds.";
}

void Combatant::stunLogic() {
  assert(stun_time != 0 && "enterHitstun has to be called first!");
  stun_clock += Game::deltaTime() / stun_time;
  stun_clock = Clamp(stun_clock, 0.0, 1.0);

  if (knockback != 0 && kb_time != 0) {
    kb_clock += Game::deltaTime() / kb_time;
    kb_clock = Clamp(kb_clock, 0.0, 1.0);
    applyKnockback(kb_clock);
  }

  stunTintLerp();

  if (stun_clock == 1.0) {
    exitHitstun();
  }
}

void Combatant::applyKnockback(float clock, float minimum) {
  float percentage = Clamp(1.0 - clock, minimum, 1.0);
  float magnitude = (knockback * percentage) * Game::deltaTime();

  if (state != DEAD && Collision::checkX(this, magnitude, kb_direction)) {
    Collision::snapX(this, kb_direction);
    stun_clock = Clamp(stun_clock, 0.75, 1.0);
  }
  else {
    position.x += magnitude * kb_direction; 
  }

  direction = static_cast<Direction>(kb_direction * -1);
  rectExCorrection(bounding_box, hurtbox);
}

void Combatant::stunTintLerp() {
  float percentage = Clamp(stun_clock / 0.30, 0.0, 1.0);
  Color end_tint = WHITE;

  unsigned char r = Lerp(start_tint.r, end_tint.r, percentage);
  unsigned char g = Lerp(start_tint.g, end_tint.g, percentage);
  unsigned char b = Lerp(start_tint.b, end_tint.b, percentage);

  tint = {r, g, b, 255};
}

void Combatant::exitHitstun() {
  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "]"
  " has exited hit stun.";
  state = CombatantState::NEUTRAL;
  stun_clock = 0.0;
  stun_time = 0;
  knockback = 0;
}

void Combatant::death() {
  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "] is"
  << " now dead!";
  start_tint = Game::palette[32];
  tint = start_tint;

  knockback = Clamp(knockback * 1.2, 90, 180);
  state = CombatantState::DEAD;
  sfx.play("death");
}

void Combatant::deathLogic() {
  assert(death_time != 0);
  death_clock += Game::deltaTime() / death_time;
  death_clock = Clamp(death_clock, 0.0, 1.0);

  if (knockback != 0) {
    applyKnockback(death_clock, 0.25);
  }

  deathTintLerp();
  deathAlphaLerp();

  if (death_clock == 1.0) {
    PLOGD << "COMBATANT: '" << name << "' [ID: " << entity_id << "] has"
    << " reached the end of their death sequence.";
    CombatHandler::raise<DeleteEntityCB>(CombatEVT::DELETE_ENTITY, 
                                         entity_id);
  }
}

void Combatant::deathTintLerp() {
  float percentage = 1.0 - Clamp(death_clock / 0.80, 0.0, 1.0);
  Color end_tint = WHITE;

  tint.r = Lerp(start_tint.r, end_tint.r, percentage);
  tint.g = Lerp(start_tint.g, end_tint.g, percentage);
  tint.b = Lerp(start_tint.b, end_tint.b, percentage);
}

void Combatant::deathAlphaLerp() {
  float unflipped = Clamp(-0.20 + death_clock, 0.0, 1.0) / 0.60;
  float percentage = Clamp(1.0 - unflipped, 0.0, 1.0);

  int value = percentage * 10;
  if (percentage == 1.0 || value % 2 != 0) {
    tint.a = 255;
  }
  else {
    tint.a = Lerp(0, 255, percentage);
  }
}

void Combatant::performAction(unique_ptr<CombatAction> &action) {
  assert(action != nullptr);
  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "]" <<
    " is performing ACTION: '" << action->name << "'";
  unique_ptr<CombatAction> old_action;
  old_action.swap(this->action);

  this->action.swap(action);
  state = CombatantState::ACTION;
  old_action.reset();
} 

void Combatant::cancelAction() {
  PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "] action"
  << " has been canceled!";
  action.reset();

  if (state != CombatantState::HIT_STUN) {
    state = CombatantState::NEUTRAL;
  }
}

void Combatant::afflictStatus(unique_ptr<StatusEffect> &effect,
                              bool hide_text) 
{
  assert(effect != nullptr);
  for (auto &status_effect : status) {
    if (effect->id == status_effect->id) {
      PLOGI << "COMBATANT: '" << name << "' [ID: " << entity_id << "]" <<
        " is already afflicted with: '" << effect->name << "'";
      effect.reset();
      return;
    }
  }

  PLOGI << "Combatant: '" << name << "' [ID: " << entity_id << "]" <<
  " has been granted StatusEffect: '" << effect->name << "'";
  effect->init(hide_text);
  status.push_back(std::move(effect));
}

void Combatant::statusLogic() {
  if (status.empty()) {
    return;
  }

  bool erased = false;
  for (auto &status_effect : status) {
    status_effect->logic();

    if (status_effect->end) {
      PLOGD << "Clearing Status Effect: '" << status_effect->name <<
        "' from memory.";
      status_effect.reset();
      erased = true;
    }
  }

  if (erased) {
    removeErasedStatus();
  } 
}

void Combatant::removeErasedStatus() {
  Status temporary;

  for (auto &status_effect : status) {
    if (status_effect != nullptr) {
      unique_ptr<StatusEffect> temp_status;
      status_effect.swap(temp_status);

      temporary.push_back(std::move(temp_status));
    }
  }

  status.clear();
  temporary.swap(status);
}

void Combatant::applyStaggerEffect(Rectangle &final) {
  bool staggered = state == HIT_STUN && stun_type == StunType::STAGGER;
  if (staggered) {
    float offset = std::sinf(GetTime() * 100);
    float percentage = 1.0 - stun_clock;
    final.x += offset * percentage;
  }
}

void Combatant::drawDebug() {
  Entity::drawDebug();
  DrawRectangleLinesEx(hurtbox.rect, 1, RED);
}
