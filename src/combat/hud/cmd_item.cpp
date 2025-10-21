#include <cassert>
#include <cstddef>
#include <raylib.h>
#include "game.h"
#include "data/session.h"
#include "base/party_member.h"
#include "utils/input.h"
#include "scenes/combat.h"
#include "combat/hud/cmd_item.h"
#include "combat/combatants/party/mary.h"
#include <plog/Log.h>


ItemCmdHud::ItemCmdHud(Vector2 position) {
  main_position = position;

  atlas = &CombatScene::cmd_atlas;
  sfx = &Game::menu_sfx;
  keybinds = &Game::settings.menu_keybinds;
}

ItemCmdHud::~ItemCmdHud() {
  if (atlas->users() != 0) {
    atlas->release();
  }

  if (sfx->users() != 0) {
    sfx->release();
  }
}

void ItemCmdHud::enable() {
  atlas->use();
  sfx->use();

  assert(player != NULL);
  Mary *mary = *player;
  mary->setEnabled(false);

  enabled = true;
  sfx->play("menu_select");
  PLOGI << "Enabled Item Command Hud.";
}

void ItemCmdHud::disable() {
  atlas->release();
  sfx->release();

  assert(player != NULL);
  Mary *mary = *player;
  mary->setEnabled(true);

  enabled = false;
  sfx->play("menu_cancel");
  PLOGI << "Disabled Item Command Hud.";
}

void ItemCmdHud::assign(Mary *&player, PartyMember *&companion,
                        Session *session) 
{
  this->player = &player;
  this->companion = &companion;
  this->session = session;
  PLOGI << "Assigned ItemCmdHud to Party.";
}

void ItemCmdHud::update() {
  if (!enabled) {
    return;
  }

  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->cancel, gamepad)) {
    disable();
  }
}

void ItemCmdHud::draw() {
  if (!enabled) {
    return;
  }
}
