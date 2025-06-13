#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "data/keybinds.h"
#include "data/session.h"
#include "utils/input.h"
#include "combat/combatants/player.h"
#include <plog/Log.h>

bool PlayerCombatant::controllable = true;
CombatKeybinds PlayerCombatant::key_bind;


PlayerCombatant::PlayerCombatant(Player *plr) : 
  Combatant(plr->name, CombatantType::PLAYER, {0, 152}, RIGHT)
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
  bool gamepad = IsGamepadAvailable(0);
  movementInput(gamepad);
}

void PlayerCombatant::movementInput(bool gamepad) {
  bool right = Input::down(key_bind.move_right, gamepad);
  bool left = Input::down(key_bind.move_left, gamepad);

  moving_x = right - left;
  moving = moving_x != 0;
}

void PlayerCombatant::update() {
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
