#include <cassert>
#include <cmath>
#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include "base/panel.h"
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "utils/math.h"
#include "utils/text.h"
#include "utils/menu.h"
#include "utils/input.h"
#include "system/sprite_atlas.h"
#include "menu/panels/dialog.h"
#include "menu/panels/crafting.h"
#include <plog/Log.h>

using std::string, std::make_unique, std::vector;


CraftingPanel::CraftingPanel(Session *session, SpriteAtlas *rest_atlas) {
  id = PanelID::CRAFTING;
  frame = LoadTexture("graphics/menu/frames/crafting.png");

  this->session = session;
  keybinds = &Game::settings.menu_keybinds;

  this->rest_atlas = rest_atlas;
  menu_atlas = &Game::menu_atlas;
  menu_atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();

  PLOGD << "Attempting to copy session inventory into array.";
  ItemID *session_inv = session->inventory;
  std::copy(session_inv, session_inv + ITEM_LIMIT, inventory.begin());
  selected_slot = inventory.begin();
  PLOGD << "CraftingPanel has been initialized.";
}

CraftingPanel::~CraftingPanel() {
  UnloadTexture(frame);
  menu_atlas->release();
  sfx->release();
}

void CraftingPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
  }
  else if (panel_mode) {
    panelLogic();
  }
  else if (!craft_mode) {
    blink_clock += Game::deltaTime();
    slotSelection();
  }
  else { 
    blink_clock += Game::deltaTime();
    itemSelection();
  }
}

void CraftingPanel::heightLerp() {
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 168 * percentage;
}

void CraftingPanel::panelLogic() {
  panel->update();

  if (!panel->terminate) {
    return;
  }

  assert(panel->selected != NULL);
  if (*panel->selected == PromptOptions::YES) {
    craftItem();
    sfx->play("menu_craft");
    craft_mode = false;
  }

  panel.reset();
  panel_mode = false;
}

void CraftingPanel::slotSelection() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(inventory, selected_slot);
    blink_clock = 0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(inventory, selected_slot);
    blink_clock = 0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    craft_mode = true;
    blink_clock = 0;
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void CraftingPanel::itemSelection() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(craft_options, selected_option);
    blink_clock = 0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(craft_options, selected_option);
    blink_clock = 0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) { 
    openCraftingDialog();
    blink_clock = 0;
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    craft_mode = false;
    blink_clock = 0;
    sfx->play("menu_cancel");
  }
}

void CraftingPanel::openCraftingDialog() {
  PLOGI << "Attempting to open crafting dialog.";

  int item_cost = getSupplyCost(*selected_option);
  PLOGD << "Item Cost: " << item_cost;

  if (session->supplies < item_cost) {
    PLOGI << "Item is too expensive!";
    sfx->play("menu_cancel");
    return;
  }

  string item_name = getName(*selected_option);
  string text = TextFormat("Spend %i supplies to craft\n"
                           "%s?", item_cost, item_name.c_str());

  vector<string> dialog = {text};
  Vector2 position = {16, 183};

  panel = make_unique<DialogPanel>(position, dialog, true);
  panel_mode = true;
}

void CraftingPanel::craftItem() {
  PLOGI << "Crafting Item...";
  ItemID item = *selected_option;
  int cost = getSupplyCost(item);

  session->supplies -= cost;

  if (*selected_slot == ItemID::NONE) {
    session->item_count++;
    assert(session->item_count <= session->item_limit);
  }

  *selected_slot = item;

  PLOGD << "Copying over changes to session inventory.";
  std::copy(inventory.begin(), inventory.end(), session->inventory);
}

void CraftingPanel::draw() {
  Rectangle source = {0, 0, 296, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);
  drawSupplyCount();

  if (state == PanelState::READY) {
    drawHelpText();
    drawInventory();
    drawOptions();
  }

  if (panel_mode) {
    panel->draw();
  }
}

void CraftingPanel::drawSupplyCount() {
  Font *sm_font = &Game::sm_font;
  int sm_size = sm_font->baseSize;
  
  Font *med_font = &Game::med_font;
  int med_size = med_font->baseSize;

  Rectangle *sprite = &rest_atlas->sprites[0];
  Color main_color = Game::palette[51];
  Color count_color = WHITE;

  float offset = 0;
  if (state != PanelState::READY) {
    offset = Math::smoothstep(16, 0, percentage);
    main_color.a = 255 * percentage;
    count_color.a = 255 * percentage;
  }

  Vector2 position = {-offset, 17};
  DrawTextureRec(rest_atlas->sheet, *sprite, position, main_color);

  position = {3 - offset, 9};
  DrawTextEx(*sm_font, "SUPPLIES", position, sm_size, -1, main_color);

  string text = TextFormat("%02i", session->supplies);
  position = TextUtils::alignRight(text.c_str(), {96 - offset, 7}, 
                                   *med_font, -2, 0);
  DrawTextEx(*med_font, text.c_str(), position, med_size, -2, 
             count_color);
}

void CraftingPanel::drawHelpText() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  string text;
  if (!craft_mode) {
    text = "Select an inventory slot.";
  }
  else { 
    text = "Select an item to craft.";
  }

  Vector2 position = {158, 37};
  DrawTextEx(*font, text.c_str(), position, txt_size, -2, WHITE);
}

void CraftingPanel::drawInventory() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Vector2 position = {81, 68};

  for (ItemID &item : inventory) {
    string name = getShortenedName(item);
    Color color = WHITE;
    Color cursor_color = WHITE;

    if (craft_mode && selected_slot == &item) {
      color = Game::palette[33];
      cursor_color = Game::palette[33];
    }
    else if (item == ItemID::NONE) {
      color = Game::palette[2];
    }

    if (selected_slot == &item) {
      drawCursor(position, cursor_color, !craft_mode);
    }

    DrawTextEx(*font, name.c_str(), position, txt_size, -2, color);
    position.y += 16;
  }
}

void CraftingPanel::drawOptions() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Vector2 position = {168, 68};

  for (ItemID &option : craft_options) {
    string name = getName(option);
    int item_cost = getSupplyCost(option);
    Color color = WHITE;

    if (session->supplies < item_cost) {
      color = Game::palette[2];
    }

    if (craft_mode && selected_option == &option) {
      drawCursor(position, WHITE, !panel_mode);
    }

    DrawTextEx(*font, name.c_str(), position, txt_size, -2, color);
    drawSupplyCost(font, txt_size, position, color, item_cost);
    position.y += 16;
  }
}

void CraftingPanel::drawSupplyCost(Font *font, int txt_size, 
                                   Vector2 position, Color color, 
                                   int cost)
{
  const char *text = TextFormat("%i", cost);
  position.x += 159;
  position = TextUtils::alignCenter(text, position, *font, -2, 0);

  DrawTextEx(*font, text, position, txt_size, -2, color);
}

void CraftingPanel::drawCursor(Vector2 position, Color color, bool blink)
{
  Rectangle *sprite = &menu_atlas->sprites[0];
  position = Vector2Add(position, {-11, 2});

  if (blink) {
    float sin_a = std::sinf(blink_clock * 2.5);
    sin_a = (sin_a / 2) + 0.5;
    color.a = 255 * sin_a;
  }

  DrawTextureRec(menu_atlas->sheet, *sprite, position, color);
}

string CraftingPanel::getShortenedName(ItemID id) {
  switch (id) {
    case ItemID::NONE: {
      return "--------";
    }
    case ItemID::I_BANDAGE: {
      return "I.Bandage";
    }
    case ItemID::M_SPLINT: {
      return "M.Splint";
    }
    case ItemID::S_BANDAGE: {
      return "S.Bandage";
    }
    case ItemID::S_WATER: {
      return "S.Water";
    }
    case ItemID::P_KILLERS: {
      return "P.Killers";
    }
    default: {
      return "INVALID";
    }
  }
}

string CraftingPanel::getName(ItemID id) {
  switch (id) {
    case ItemID::I_BANDAGE: {
      return "Improvised Bandage";
    }
    case ItemID::M_SPLINT: {
      return "Makeshift Splint";
    }
    case ItemID::S_BANDAGE: {
      return "Sterilized Bandage";
    }
    case ItemID::S_WATER: {
      return "Sparkling Water";
    }
    case ItemID::P_KILLERS: {
      return "Painkillers";
    }
    default: {
      return "INVALID";
    }
  }
}

int CraftingPanel::getSupplyCost(ItemID id) {
  switch (id) {
    case ItemID::I_BANDAGE: {
      return 15;
    }
    case ItemID::M_SPLINT: {
      return 25;
    }
    case ItemID::S_BANDAGE: {
      return 30;
    }
    case ItemID::S_WATER: {
      return 15;
    }
    case ItemID::P_KILLERS: {
      return 10;
    }
    default: {
      return -1;
    }
  }
}
