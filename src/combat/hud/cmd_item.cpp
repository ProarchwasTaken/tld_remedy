#include <string>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include "base/combatant.h"
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

  target_position = Vector2Add(position, {-32, -22});
  target_color = Game::palette[33];

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

  party = {player, companion};
  PLOGI << "Assigned ItemCmdHud to Party.";
}

void ItemCmdHud::enable() {
  assert(player != NULL);
  Mary *mary = *player;

  if (session->item_count == 0) {
    PLOGI << "Player has no usable items!";
    mary->sfx.play("action_denied");
    return;
  }

  if (mary->state == ACTION && mary->action->id == ActionID::USE_ITEM) {
    PLOGI << "Player is already using an item!";
    mary->sfx.play("action_denied");
    return;
  }

  mary->setEnabled(false);

  std::copy(session->inventory, session->inventory + 8, options.begin());
  updateSelected();

  target = party.begin();
  
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
    ItemID item = options.at(index);

    if (item != ItemID::NONE) {
      selected = options.begin() + index;
      PLOGD << "Selected set to: " << static_cast<int>(item);
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
  target_mode = false;
  PLOGI << "Disabled Item Command Hud.";
}

void ItemCmdHud::update() {
  if (!enabled) {
    return;
  }

  Mary *mary = *player;

  bool gamepad = IsGamepadAvailable(0);
  mary->movementInput(gamepad);

  if (mary->state == HIT_STUN || mary->state == DEAD) {
    disable();
    sfx->play("menu_cancel"); 
    return;
  }

  if (target_mode && !enterTargetMode()) {
    target_mode = false;
    opt_switch_clock = 0.0;
    target = party.begin();
    sfx->play("menu_cancel"); 
  }

  if (!target_mode) {
    optionNavigation();
  }
  else {
    targetNavigation();
  }

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
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    selectOption();
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    disable();
    sfx->play("menu_cancel");
  }
}

void ItemCmdHud::selectOption() {
  if (enterTargetMode()) {
    target_mode = true;
    opt_switch_clock = 0.0;
  }
  else {
    useItem();
    disable();
  }
}

bool ItemCmdHud::enterTargetMode() {
  if (*companion == NULL) {
    return false;
  }

  PartyMember *companion = *this->companion;
  if (companion->state == DEAD) {
    return false;
  }
  else {
    return true;
  }
}

void ItemCmdHud::targetNavigation() {
  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(party, target);
    opt_switch_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(party, target);
    opt_switch_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    target_mode = false;
    opt_switch_clock = 0.0;
    sfx->play("menu_cancel");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    useItem();
    disable();
    sfx->play("menu_select");
  }
}

void ItemCmdHud::useItem() {
  Mary *mary = *player;

  float multiplier = 2 - mary->speed_multiplier;
  float use_time = 0.5 * multiplier;

  assert(selected != NULL);
  assert(*selected != ItemID::NONE);

  PartyMember *target = *this->target;
  PLOGD << "Target Selected: '" << target->name << "'";
  PLOGD << "Use Item: " << use_time;
  mary->useItem(*selected, use_time, target);
}

void ItemCmdHud::draw() {
  if (!enabled) {
    return;
  }

  Font *font = &Game::sm_font;
  int txt_size = font->baseSize;

  drawNamePlate("ITEMS", font, txt_size, main_position, main_color,
                Game::palette[12]);
  drawOptions(font, txt_size);

  if (target_mode) {
    assert(*companion != NULL);
    drawNamePlate("PARTY", font, txt_size, target_position, target_color, 
                  Game::palette[32]);
    drawTargetOptions(font, txt_size);
  }
}

void ItemCmdHud::drawNamePlate(const char *text, Font *font, 
                               int txt_size, Vector2 position, 
                               Color main_color, Color col_pattern) 
{
  Vector2 frame_position = Vector2Add(position, {12, 0});
  DrawTextureRec(atlas->sheet, atlas->sprites[0], frame_position, 
                 main_color);
  DrawTextureRec(atlas->sheet, atlas->sprites[1], frame_position, 
                 col_pattern);

  Vector2 name_position = Vector2Add(position, {66, 1});
  name_position = TextUtils::alignRight(text, name_position, *font, 
                                        -3, 0);

  DrawTextEx(*font, text, name_position, txt_size, -3, WHITE);
}

void ItemCmdHud::drawOptions(Font *font, int txt_size) {
  Rectangle *sprite = &atlas->sprites[2];
  Vector2 position = main_position;

  for (int index = 0; index < session->item_limit; index++) {
    ItemID *item = &options.at(index);
    if (*item == ItemID::NONE) {
      continue;
    }

    if (item == selected) {
      float percentage = Clamp(opt_switch_clock + target_mode, 0.0, 1.0);
      position.x -= 8 * percentage;
    }
    position.y += 11;

    DrawTextureRec(atlas->sheet, *sprite, position, main_color);
    drawOptionText(*item, position, font, txt_size);
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
    case ItemID::S_BANDAGE: {
      name = "S.Bandage";
      break;
    }
    case ItemID::S_WATER: {
      name = "S.Water";
      break;
    }
    case ItemID::P_KILLERS: {
      name = "P.Killers";
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

void ItemCmdHud::drawTargetOptions(Font *font, int txt_size) {
  Rectangle *sprite = &atlas->sprites[2];
  Vector2 position = target_position;

  for (int index = 0; index < 2; index++) {
    if (party.begin() + index == target) {
      position.x -= 8 * opt_switch_clock;
    }
    position.y += 11;

    DrawTextureRec(atlas->sheet, *sprite, position, target_color);

    PartyMember *member = party.at(index);
    drawMemberName(member, position, font, txt_size);
    
    position.x = target_position.x; 
  }
}

void ItemCmdHud::drawMemberName(PartyMember *member, Vector2 position,
                                Font *font, int txt_size)
{
  const char *name = member->name.c_str();
  position = Vector2Add(position, {59, 1});
  position = TextUtils::alignRight(name, position, *font, -3, 0);
  DrawTextEx(*font, name, position, txt_size, -3, WHITE);
}

