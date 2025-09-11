#include <cassert>
#include <cstddef>
#include <raylib.h>
#include <memory>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "base/party_member.h"
#include "data/combatant_event.h"
#include "data/animation.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "utils/input.h"
#include "utils/animation.h"
#include "utils/collision.h"
#include "combat/system/cbt_handler.h"
#include "combat/sub_weapons/knife.h"
#include "combat/actions/attack.h"
#include "combat/actions/ghost_step.h"
#include "combat/actions/evade.h"
#include "combat/combatants/party/mary.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique;
bool Mary::controllable = true;

SpriteAtlas Mary::atlas("combatants", "mary_combatant");


Mary::Mary(Player *plr): 
  PartyMember("Mary", PartyMemberID::MARY, {-64, 152})
{
  keybinds = &Game::settings.combat_keybinds;
  important = true;

  life = plr->life;
  max_life = plr->max_life;
  critical_life = life < max_life * LOW_LIFE_THRESHOLD;

  init_morale = plr->init_morale;
  morale = init_morale;
  max_morale = plr->max_morale;

  offense = plr->offense;
  defense = plr->defense;
  intimid = plr->intimid;
  persist = plr->persist;

  assignSubWeapon(plr->weapon_id);
  afflictPersistent(plr->status);

  int framerate = Game::settings.framerate;
  float quotient = Game::TARGET_FPS / framerate;
  buffer_lifetime = buffer_lifetime * quotient;
  PLOGD << "Buffer Lifetime: " << buffer_lifetime;

  bounding_box.scale = {64, 64};
  bounding_box.offset = {-32, -64};
  hurtbox.scale = {16, 56};
  hurtbox.offset = {-8, -58};
  rectExCorrection(bounding_box, hurtbox);

  atlas.use();
  sprite = &atlas.sprites[0];
}

Mary::~Mary() {
  atlas.release();
}

void Mary::assignSubWeapon(SubWeaponID id) {
  PLOGI << "Assigning Sub-Weapon.";

  switch (id) {
    case SubWeaponID::KNIFE: {
      sub_weapon = make_unique<Knife>(this);
      break;
    }
  }

  tech1_name = sub_weapon->tech1_name;
  tech1_cost = sub_weapon->tech1_cost;
  tech1_type = sub_weapon->tech1_type;

  tech2_name = sub_weapon->tech2_name;
  tech2_cost = sub_weapon->tech2_cost;
  tech2_type = sub_weapon->tech2_type;
}

void Mary::setControllable(bool value) {
  controllable = value;

  if (controllable) {
    PLOGI << "Control has been given to the player.";
  }
  else {
    PLOGI << "Control has been revoked from the player.";
  }
}

void Mary::behavior() {
  if (!controllable) {
    return;
  }

  eventHandling();

  bool gamepad = IsGamepadAvailable(0);
  movementInput(gamepad);
  actionInput(gamepad);

  if (state == CombatantState::NEUTRAL || canCancel()) {
    readActionBuffer();
  }
}

void Mary::eventHandling() {
  EventPool<CombatantEvent> *event_pool = CombatantHandler::get();

  for (auto &event : *event_pool) {
    if (event == nullptr) {
      continue;
    }

    if (event->event_type == CombatantEVT::TOOK_DAMAGE) {
      TookDamageCBT *dmg_event = static_cast<TookDamageCBT*>(event.get());
      damageHandling(dmg_event);
    }
  }
}

void Mary::damageHandling(TookDamageCBT *event) {
  if (target != NULL) {
    return;
  }

  Combatant *potential_target = NULL;
  if (event->sender == this) {
    assert(event->assailant != NULL);
    potential_target = event->assailant;
  }
  else if (event->assailant == this) {
    assert(event->sender->entity_type == COMBATANT);
    potential_target = static_cast<Combatant*>(event->sender);
  }
  else {
    return;
  }

  PLOGD << "Acknowledging damage event of which the Player Combatant was"
    " involved in.";
  float distance = distanceTo(potential_target);
  if (distance <= 96) {
    target = potential_target;
    PLOGI << "Now targeting: '" << target->name << "' [ID: " << 
    target->entity_id << "]";
  }
}

void Mary::movementInput(bool gamepad) {
  bool right = Input::down(keybinds->move_right, gamepad);
  bool left = Input::down(keybinds->move_left, gamepad);

  moving_x = right - left;
  moving = moving_x != 0;
}

void Mary::actionInput(bool gamepad) {
  if (buffer != MaryAction::NONE) {
    return;
  }

  if (Input::pressed(keybinds->attack, gamepad)) {
    PLOGI << "Sending Attack input to buffer.";
    buffer = MaryAction::ATTACK;
  }
  else if (Input::pressed(keybinds->defensive, gamepad)) {
    defensiveActionInput(gamepad);
  }
  else if (Input::pressed(keybinds->light_tech, gamepad)) {
    PLOGI << "Sending Light Tech input to buffer.";
    buffer = MaryAction::LIGHT_TECH;
  }
  else if (Input::pressed(keybinds->heavy_tech, gamepad)) {
    PLOGI << "Sending Heavy Tech input to buffer.";
    buffer = MaryAction::HEAVY_TECH;
  }
  else {
    return;
  }

  if (state != NEUTRAL) {
    PLOGD << "Input has been added to buffer while the player is not in"
      << " neutral!";
  }
}

void Mary::defensiveActionInput(bool gamepad) {
  if (moving) {
    PLOGI << "Sending GhostStep input to buffer.";
    buffer = MaryAction::GHOST_STEP;
    return;
  }
  else if (Input::down(keybinds->down, gamepad)) {
    PLOGI << "Sending Evade input to buffer.";
    buffer = MaryAction::EVADE;
    return;
  }
}

bool Mary::canCancel(bool ignore_buffer) {
  bool buffer_empty = !ignore_buffer && buffer == MaryAction::NONE;
  if (state != CombatantState::ACTION || buffer_empty) {
    return false;
  }

  bool in_end_lag = action->phase == ActionPhase::END_LAG;
  if (!in_end_lag) {
    return false;
  }

  ActionID action_id = action->id;
  if (action_id == ActionID::EVADE) {
    Evade *evade_action = static_cast<Evade*>(action.get());
    return buffer != MaryAction::EVADE && evade_action->evaded_attack;
  }
  else if (action_id == ActionID::GHOST_STEP) {
    return buffer != MaryAction::GHOST_STEP;
  }
  else {
    return buffer == MaryAction::GHOST_STEP;
  }
}

void Mary::readActionBuffer() {
  if (buffer == MaryAction::NONE) {
    return;
  }
  PLOGI << "Reading action buffer.";
  PLOGD << "Time since input was added to buffer: " 
    << buffer_lifetime * buffer_clock;
  PLOGD << "Timing percentage: " << buffer_clock;

  unique_ptr<CombatAction> action;
  switch (buffer) {
    default: {
      assert(buffer != MaryAction::NONE);
      break;
    }
    case MaryAction::ATTACK: {
      RectEx hitbox;
      hitbox.scale = {28, 8};
      hitbox.offset = {-14 + (14.0f * direction), -50};

      action = make_unique<Attack>(this, atlas, hitbox, atk_set);
      break;
    }
    case MaryAction::GHOST_STEP: {
      if (!critical_life) {
        increaseExhaustion(gs_cost);
        action = make_unique<GhostStep>(this, atlas, moving_x, gs_set);
      }

      break;
    }
    case MaryAction::EVADE: {
      if (!critical_life) {
        RectEx hitbox;
        hitbox.scale = {8, 46};
        hitbox.offset = {-4 + (8.0f * direction), -48};

        action = make_unique<Evade>(this, atlas, hitbox, ev_set);
      }
      break;
    }
    case MaryAction::LIGHT_TECH: {
      bool usable = sub_weapon->lightTechCondition();
      if (usable) {
        action = sub_weapon->lightTechnique();
      }

      break;
    }
    case MaryAction::HEAVY_TECH: {
      bool usable = sub_weapon->heavyTechCondition();
      if (usable) {
        action = sub_weapon->heavyTechnique();
      }

      break;
    }
  }

  buffer = MaryAction::NONE;
  buffer_clock = 0.0;

  if (action != nullptr) {  
    performAction(action);
  }
  else {
    sfx.play("action_denied"); 
  }
}

void Mary::update() {
  tintFlash();

  switch (state) {
    case CombatantState::NEUTRAL: {
      if (exhaustion != 0) {
        depleteExhaustion();
      }

      neutralLogic();
      break;
    }
    case CombatantState::ACTION: {
      action->logic();
      break;
    }
    case CombatantState::HIT_STUN: {
      stunLogic();
      sprite = getStunSprite();
      break;
    }
    case CombatantState::DEAD: {
      if (deathClock() == 0) {
        sprite = &atlas.sprites[7];
      }
      else {
        SpriteAnimation::play(animation, &anim_dead, false);
        sprite = &atlas.sprites[*animation->current];    
      }

      deathLogic();
      break;
    }
  }

  statusLogic();
  targetLogic();

  if (buffer != MaryAction::NONE) {
    bufferTimer();
  }
}

void Mary::neutralLogic() {
  float old_x = position.x;
  movement();

  Animation *next_anim;
  has_moved = old_x != position.x;
  if (has_moved) {
    next_anim = &anim_move;
    float difference = 1.0 - speed_multiplier;
    float percentage = 1.0 + difference;
    next_anim->frame_duration = anim_move_speed * percentage;

    last_moved = 0.0;
    rectExCorrection(bounding_box, hurtbox);
  }
  else {
    last_moved += Game::deltaTime();
    next_anim = getIdleAnim();
  }

  SpriteAnimation::play(animation, next_anim, true);
  sprite = &atlas.sprites[*animation->current];
}

void Mary::bufferTimer() {
  buffer_clock += Game::deltaTime() / buffer_lifetime;
  if (buffer_clock >= 1.0) {
    PLOGI << "Resetting action buffer.";
    buffer = MaryAction::NONE;
    buffer_clock = 0.0;
  }
}

void Mary::targetLogic() {
  if (target == NULL) {
    return;
  }

  float distance = distanceTo(target); 
  if (distance > 128) {
    PLOGI << "Target: '" << target->name << "' [ID: " << 
      target->entity_id << "] is outside the player's range.";
    target = NULL;
  }
}

void Mary::movement() {
  if (!moving) {
    return;
  }

  direction = static_cast<Direction>(moving_x);

  float speed = default_speed * speed_multiplier;
  float magnitude = speed * Game::deltaTime();

  if (Collision::checkX(this, magnitude, direction)) {
    Collision::snapX(this, direction);
  }
  else {
    position.x += magnitude * direction;
  }
}

Animation *Mary::getIdleAnim() {
  if (!critical_life || last_moved < 0.35) {
    return &anim_idle;
  }
  else {
    return &anim_crit;
  }
}

Rectangle *Mary::getStunSprite() {
  if (damage_type == DamageType::LIFE) {
    return &atlas.sprites[7];
  }
  else {
    return &atlas.sprites[8];
  }
}

void Mary::draw() {
  assert(sprite != NULL);

  Rectangle final = *sprite;
  final.width = final.width * direction;

  DrawTexturePro(atlas.sheet, final, bounding_box.rect, {0, 0}, 0, tint);
}

void Mary::drawDebug() {
  Combatant::drawDebug();

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
