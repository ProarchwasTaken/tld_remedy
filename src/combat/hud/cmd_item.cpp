#include <raylib.h>
#include "game.h"
#include "data/session.h"
#include "base/party_member.h"
#include "scenes/combat.h"
#include "combat/hud/cmd_item.h"
#include <plog/Log.h>


ItemCmdHud::ItemCmdHud(Vector2 position, Session *session) {
  main_position = position;
  this->session = session;

  atlas = &CombatScene::cmd_atlas;
  sfx = &Game::menu_sfx;
  keybinds = &Game::settings.menu_keybinds;
}

void ItemCmdHud::enable() {
  atlas->use();
  sfx->use();

  enabled = true;
  PLOGI << "Enabled Item Command Hud.";
}

void ItemCmdHud::disable() {
  atlas->release();
  sfx->release();

  enabled = false;
  PLOGI << "Disabled Item Command Hud.";
}

void ItemCmdHud::assign(PartyMember *player, PartyMember *companion) {
  this->player = &player;
  this->companion = &companion;
  PLOGI << "Assigned ItemCmdHud to Party.";
}

void ItemCmdHud::update() {
  if (!enabled) {
    return;
  }
}

void ItemCmdHud::draw() {
  if (!enabled) {
    return;
  }
}
