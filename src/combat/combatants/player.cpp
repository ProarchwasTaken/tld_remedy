#include <raylib.h>
#include <memory>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "utils/input.h"
#include "combat/actions/attack.h"
#include "combat/combatants/player.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique;
bool PlayerCombatant::controllable = true;
CombatKeybinds PlayerCombatant::key_bind;


PlayerCombatant::PlayerCombatant(Player *plr) : 
  Combatant(plr->name, CombatantType::PLAYER, {-64, 152}, RIGHT)
{
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
}

void PlayerCombatant::setControllable(bool value) {
  controllable = value;

  if (controllable) {
    PLOGI << "Control has been given to the player.";
  }
  else {
    PLOGI << "Control has been revoked from the player.";
  }
}

void PlayerCombatant::behavior() {
  if (state == CombatantState::NEUTRAL) {
    bool gamepad = IsGamepadAvailable(0);
    movementInput(gamepad);
    actionInput(gamepad);
  }
}

void PlayerCombatant::movementInput(bool gamepad) {
  bool right = Input::down(key_bind.move_right, gamepad);
  bool left = Input::down(key_bind.move_left, gamepad);

  moving_x = right - left;
  moving = moving_x != 0;
}

void PlayerCombatant::actionInput(bool gamepad) {
  unique_ptr<CombatAction> action;

  if (Input::pressed(key_bind.attack, gamepad)) {
    RectEx hitbox;
    hitbox.scale = {32, 16};
    hitbox.offset = {-16 + (16.0f * direction), -40};

    action = make_unique<Attack>(this, hitbox);
  }

  if (action != nullptr) {
    performAction(action);
  }
}

void PlayerCombatant::update() {
  switch (state) {
    case CombatantState::NEUTRAL: {
      neutralLogic();
      break;
    }
    case CombatantState::ACTION: {
      action->logic();
    }
    default: {

    }
  }
}

void PlayerCombatant::neutralLogic() {
  float old_x = position.x;
  movement();

  has_moved = old_x != position.x;
  if (has_moved) {
    rectExCorrection(bounding_box, hurtbox);
  }
}

void PlayerCombatant::movement() {
  if (!moving) {
    return;
  }

  direction = static_cast<Direction>(moving_x);

  float speed = default_speed * speed_multiplier;
  position.x += (speed * direction) * Game::deltaTime();
}

void PlayerCombatant::draw() {
}

void PlayerCombatant::drawDebug() {
  Combatant::drawDebug();

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
