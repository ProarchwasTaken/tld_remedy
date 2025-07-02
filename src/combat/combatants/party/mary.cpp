#include <cassert>
#include <cstddef>
#include <raylib.h>
#include <memory>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "base/party_member.h"
#include "data/keybinds.h"
#include "data/animation.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "utils/input.h"
#include "utils/animation.h"
#include "combat/actions/attack.h"
#include "combat/combatants/party/mary.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique;
bool Mary::controllable = true;

CombatKeybinds Mary::key_bind;
SpriteAtlas Mary::atlas("combatants", "mary_combatant");


Mary::Mary(Player *plr): 
  PartyMember("Mary", PartyMemberID::MARY, {-64, 152})
{
  important = true;
  life = plr->life;
  max_life = plr->max_life;

  init_morale = plr->init_morale;
  morale = init_morale;
  max_morale = plr->max_morale;

  offense = plr->offense;
  defense = plr->defense;
  intimid = plr->intimid;
  persist = plr->persist;

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
  if (state == CombatantState::NEUTRAL) {
    bool gamepad = IsGamepadAvailable(0);
    movementInput(gamepad);
    actionInput(gamepad);
  }
}

void Mary::movementInput(bool gamepad) {
  bool right = Input::down(key_bind.move_right, gamepad);
  bool left = Input::down(key_bind.move_left, gamepad);

  moving_x = right - left;
  moving = moving_x != 0;
}

void Mary::actionInput(bool gamepad) {
  unique_ptr<CombatAction> action;

  if (Input::pressed(key_bind.attack, gamepad)) {
    RectEx hitbox;
    hitbox.scale = {28, 8};
    hitbox.offset = {-14 + (14.0f * direction), -50};

    action = make_unique<Attack>(this, atlas, hitbox, atk_set);
  }

  if (action != nullptr) {
    performAction(action);
  }
}

void Mary::update() {
  switch (state) {
    case CombatantState::NEUTRAL: {
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
      break;
    }
  }
}

void Mary::neutralLogic() {
  float old_x = position.x;
  movement();

  Animation *next_anim;
  has_moved = old_x != position.x;
  if (has_moved) {
    next_anim = &anim_move;
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

void Mary::movement() {
  if (!moving) {
    return;
  }

  direction = static_cast<Direction>(moving_x);

  float speed = default_speed * speed_multiplier;
  float magnitude = (speed * direction) * Game::deltaTime();
  float half_scale = (hurtbox.scale.x / 2) * direction;
  float offset = magnitude + half_scale;

  float bounds = 512;
  if (position.x + offset > bounds) {
    position.x = bounds - half_scale;
  }
  else if (position.x + offset < -bounds) {
    position.x = -bounds - half_scale;
  }
  else {
    position.x += magnitude;
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

  DrawTexturePro(atlas.sheet, final, bounding_box.rect, {0, 0}, 0, WHITE);
}

void Mary::drawDebug() {
  Combatant::drawDebug();

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
