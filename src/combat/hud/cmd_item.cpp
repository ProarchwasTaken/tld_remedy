#include <string>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "base/party_member.h"
#include "utils/input.h"
#include "utils/text.h"
#include "utils/menu.h"
#include "scenes/combat.h"
#include "combat/hud/cmd_item.h"
#include "combat/combatants/party/mary.h"
#include <plog/Log.h>
#include <string>

using std::string;
float ItemCmdHud::base_y = 0;


ItemCmdHud::ItemCmdHud(Vector2 position) {
  main_position = position;
  base_y = position.y;
  main_color = Game::palette[13];

  atlas = &CombatScene::cmd_atlas;
  sfx = &Game::menu_sfx;
  keybinds = &Game::settings.menu_keybinds;
}

ItemCmdHud::~ItemCmdHud() {
  if (enabled) {
    atlas->release();
    sfx->release();
  }
}

void ItemCmdHud::assign(Mary *&player, PartyMember *&companion,
                        Session *session) 
{
  this->player = &player;
  this->companion = &companion;
  this->session = session;
  PLOGI << "Assigned ItemCmdHud to Party.";
}

void ItemCmdHud::enable() {
  if (session->item_count == 0) {
    PLOGI << "Player has no usable items!";
    Combatant::sfx.play("action_denied");
    return;
  }

  assert(player != NULL);
  Mary *mary = *player;
  mary->setEnabled(false);

  std::copy(session->inventory, session->inventory + 8, options.begin());
  updateSelected();
  
  main_position.y = base_y - (11 * session->item_count);
  opt_switch_clock = 0;

  atlas->use();
  sfx->use();

  enabled = true;
  sfx->play("menu_select");
  PLOGI << "Enabled Item Command Hud.";
}

void ItemCmdHud::updateSelected() {
  selected = NULL;

  for (int index = 0; index < session->item_limit; index++) {
    ItemID *item = &options.at(index);

    if (*item != ItemID::NONE) {
      selected = options.begin() + index;
      PLOGD << "Selected set to: " << static_cast<int>(*item);
      break;
    }
  }

  assert(selected != NULL);
}

void ItemCmdHud::disable() {
  atlas->release();
  sfx->release();

  assert(player != NULL);
  Mary *mary = *player;
  mary->setEnabled(true);

  enabled = false;
  PLOGI << "Disabled Item Command Hud.";
}

void ItemCmdHud::update() {
  if (!enabled) {
    return;
  }

  optionNavigation();

  if (opt_switch_clock != 1.0) {
    opt_switch_clock += Game::deltaTime() / opt_switch_time;
    opt_switch_clock = Clamp(opt_switch_clock, 0.0, 1.0);
  }
}

void ItemCmdHud::optionNavigation() {
  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(options, selected, &disallowed);
    opt_switch_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected, &disallowed);
    opt_switch_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    disable();
    sfx->play("menu_cancel");
  }
}

void ItemCmdHud::draw() {
  if (!enabled) {
    return;
  }

  Font *font = &Game::sm_font;
  int txt_size = font->baseSize;

  drawNamePlate(font, txt_size);
  drawOptions(font, txt_size);
}

void ItemCmdHud::drawNamePlate(Font *font, int txt_size) {
  Vector2 position = Vector2Add(main_position, {12, 0});
  Color col_pattern = Game::palette[12];

  DrawTextureRec(atlas->sheet, atlas->sprites[0], position, main_color);
  DrawTextureRec(atlas->sheet, atlas->sprites[1], position, col_pattern);

  Vector2 name_position = Vector2Add(main_position, {66, 1});
  name_position = TextUtils::alignRight("ITEMS", name_position, *font, 
                                        -3, 0);

  DrawTextEx(*font, "ITEMS", name_position, txt_size, -3, WHITE);
}

void ItemCmdHud::drawOptions(Font *font, int txt_size) {
  Rectangle *sprite = &atlas->sprites[2];
  Vector2 position = main_position;

  for (int index = 0; index < session->item_count; index++) {
    ItemID item = options.at(index);
    if (item == ItemID::NONE) {
      continue;
    }

    if (options.begin() + index == selected) {
      position.x -= 8 * opt_switch_clock;
    }
    position.y += 11;

    DrawTextureRec(atlas->sheet, *sprite, position, main_color);
    drawOptionText(item, position, font, txt_size);
    position.x = main_position.x;
  }
}

void ItemCmdHud::drawOptionText(ItemID item, Vector2 position, 
                                Font *font, int txt_size) 
{
  string name;
  switch (item) {
    case ItemID::I_BANDAGE: {
      name = "I.Bandage";
      break;
    }
    case ItemID::M_SPLINT: {
      name = "M.Splint";
      break;
    }
    default: {
      assert(item != ItemID::NONE);
    }
  }

  position = Vector2Add(position, {59, 1});
  position = TextUtils::alignRight(name.c_str(), position, *font, -3, 0);

  DrawTextEx(*font, name.c_str(), position, txt_size, -3, WHITE);
}
