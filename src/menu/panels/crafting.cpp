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
#include "utils/items.h"
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
  swap_slot = inventory.begin();
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
    return;
  }
  else if (panel_mode) {
    panelLogic();
    return;
  }

  if (!craft_mode && !swap_mode) {
    slotSelection();
  }
  else if (swap_mode) {
    swapSlotSelection();
  }
  else { 
    optionSelection();
  }

  blink_clock += Game::deltaTime();
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

  if (panel->terminate) {
    promptHandling();
    panel.reset();
    panel_mode = false;
  }
}

void CraftingPanel::promptHandling() {
  assert(panel->selected != NULL);
  if (*panel->selected != PromptOptions::YES) {
    return;
  }

  assert(*selected_option != MOVE_ITEM);
  if (*selected_option == RECYCLE_ITEM) {
    recycleItem();
  }
  else {
    craftItem();
  }

  selected_option = options.begin();
  craft_mode = false;
  sfx->play("menu_craft");
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

void CraftingPanel::swapSlotSelection() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(inventory, swap_slot);
    blink_clock = 0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(inventory, swap_slot);
    blink_clock = 0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    swapItemSlots();
    blink_clock = 0;
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    swap_slot = inventory.begin();
    swap_mode = false;
    blink_clock = 0;
    sfx->play("menu_cancel");
  }
}

void CraftingPanel::optionSelection() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(options, selected_option);
    blink_clock = 0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected_option);
    blink_clock = 0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) { 
    selectOption();
    blink_clock = 0;
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    selected_option = options.begin();
    craft_mode = false;
    blink_clock = 0;
    sfx->play("menu_cancel");
  }
}

void CraftingPanel::selectOption() {
  switch (*selected_option) {
    case CraftOptions::RECYCLE_ITEM: {
      openRecycleDialog(*selected_slot);
      break;
    }
    case CraftOptions::MOVE_ITEM: {
      enterSwapMode();
      break;
    }
    default: {
      ItemID item = static_cast<ItemID>(*selected_option);
      openCraftingDialog(item);
    }
  }
}

void CraftingPanel::openCraftingDialog(ItemID item) {
  PLOGI << "Attempting to open crafting dialog.";

  int item_cost = getSupplyCost(item);
  assert(item_cost != -1);
  PLOGD << "Item Cost: " << item_cost;

  if (session->supplies < item_cost) {
    PLOGI << "Item is too expensive!";
    sfx->play("menu_cancel");
    return;
  }

  string item_name = ItemUtils::getName(item);
  string text = TextFormat("Spend %i supplies to craft\n"
                           "%s?", item_cost, item_name.c_str());

  vector<string> dialog = {text};
  Vector2 position = {16, 183};

  panel = make_unique<DialogPanel>(position, dialog, true);
  panel_mode = true;
}

void CraftingPanel::openRecycleDialog(ItemID item) {
  PLOGI << "Attempting to open recycle dialog.";
  if (*selected_slot == ItemID::NONE) {
    PLOGI << "You can't recycle an empty item slot!";
    sfx->play("menu_cancel");
    return;
  }

  string item_name = ItemUtils::getName(*selected_slot);
  int item_refund = getSupplyRefund(*selected_slot);

  string text = TextFormat("Recycle %s to get\n"
                           "%i supplies back?", 
                           item_name.c_str(), item_refund);

  vector<string> dialog = {text};
  Vector2 position = {16, 183};

  panel = make_unique<DialogPanel>(position, dialog, true);
  panel_mode = true;
}

void CraftingPanel::enterSwapMode() {
  PLOGI << "Attempting to enter Swap Mode.";
  if (*selected_slot != ItemID::NONE) {
    swap_mode = true;
    blink_clock = 0;
    sfx->play("menu_select");
  }
  else {
    PLOGI << "Can't enter swap mode with an empty slot!";
    sfx->play("menu_cancel");
  }
}

void CraftingPanel::craftItem() {
  PLOGI << "Crafting Item...";
  ItemID item = static_cast<ItemID>(*selected_option);

  int cost = getSupplyCost(item);
  assert(cost != -1);

  session->supplies -= cost;

  if (*selected_slot == ItemID::NONE) {
    session->item_count++;
    assert(session->item_count <= session->item_limit);
  }

  *selected_slot = item;

  PLOGD << "Copying over changes to session inventory.";
  std::copy(inventory.begin(), inventory.end(), session->inventory);
}

void CraftingPanel::recycleItem() {
  PLOGI << "Recycling item...";

  assert(*selected_slot != ItemID::NONE);
  int refund = getSupplyRefund(*selected_slot);
  session->supplies += refund;

  *selected_slot = ItemID::NONE;
  session->item_count--;
  assert(session->item_count >= 0);

  PLOGD << "Copying over changes to session inventory.";
  std::copy(inventory.begin(), inventory.end(), session->inventory);
}

void CraftingPanel::swapItemSlots() {
  PLOGI << "Attempting to swap items slots.";
  if (selected_slot == swap_slot) {
    PLOGI << "Can't swap with the same slot!";
    sfx->play("menu_cancel");
    return;
  }

  ItemID to_be_swapped = *swap_slot;
  *swap_slot = *selected_slot;
  *selected_slot = to_be_swapped;

  selected_slot = swap_slot;
  swap_slot = inventory.begin();
  selected_option = options.begin();

  craft_mode = false;
  swap_mode = false;

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

  if (*selected_slot == ItemID::NONE) {
    return;
  }

  if (*selected_option == CraftOptions::RECYCLE_ITEM) {
    int refund = getSupplyRefund(*selected_slot);
    text = TextFormat("+%i", refund);
    position = {96, 7};
    DrawTextEx(*med_font, text.c_str(), position, med_size, -2, 
               Game::palette[14]);
  }
}

void CraftingPanel::drawHelpText() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  string text;
  if (!craft_mode) {
    text = "Select an inventory slot.";
  }
  else if (!swap_mode) { 
    text = "Select what you want to do.";
  }
  else {
    text = "To which slot?";
  }

  Vector2 position = {158, 37};
  DrawTextEx(*font, text.c_str(), position, txt_size, -2, WHITE);
}

void CraftingPanel::drawInventory() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Vector2 position = {81, 68};

  for (ItemID &item : inventory) {
    string name = ItemUtils::getShortened(item);
    Color color = WHITE;
    Color cursor_color = WHITE;

    if (craft_mode && selected_slot == &item) {
      color = Game::palette[33];
      cursor_color = Game::palette[33];
    }
    else if (swap_mode && swap_slot == &item) {
      color = Game::palette[22];
      cursor_color = Game::palette[22];
    }
    else if (item == ItemID::NONE) {
      color = Game::palette[2];
    }

    if (selected_slot == &item) {
      drawCursor(position, cursor_color, !craft_mode);
    }
    else if (swap_mode && swap_slot == &item) {
      drawCursor(position, cursor_color, true); 
    }

    DrawTextEx(*font, name.c_str(), position, txt_size, -2, color);
    position.y += 16;
  }
}

void CraftingPanel::drawOptions() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Vector2 position = {168, 68};

  for (CraftOptions &option : options) {
    switch (option) {
      case CraftOptions::RECYCLE_ITEM:
      case CraftOptions::MOVE_ITEM: {
        drawMiscOption(font, txt_size, position, option);
        break;
      }
      default: {
        ItemID item = static_cast<ItemID>(option);
        drawCraftOption(font, txt_size, position, item);
      }
    }

    if (craft_mode && selected_option == &option) {
      bool should_blink = !panel_mode && !swap_mode;
      drawCursor(position, WHITE, should_blink);
    }
    position.y += 16;
  }
}

void CraftingPanel::drawMiscOption(Font *font, int txt_size, 
                                   Vector2 position, CraftOptions option)
{
  string name;
  switch (option) {
    case RECYCLE_ITEM: {
      name = "RECYCLE ITEM";
      break;
    }
    case MOVE_ITEM: {
      name = "MOVE ITEM";
      break;
    }
    default: {

    }
  }

  Color color = WHITE;
  if (*selected_slot == ItemID::NONE) {
    color = Game::palette[2];
  }

  assert(!name.empty());
  DrawTextEx(*font, name.c_str(), position, txt_size, -2, color);
}

void CraftingPanel::drawCraftOption(Font *font, int txt_size, 
                                    Vector2 position, ItemID item)
{
  string name = ItemUtils::getName(item);

  int item_cost = getSupplyCost(item);
  assert(item_cost != -1);

  Color color = WHITE;

  if (session->supplies < item_cost) {
    color = Game::palette[2];
  }

  DrawTextEx(*font, name.c_str(), position, txt_size, -2, color);
  drawSupplyCost(font, txt_size, position, color, item_cost);
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

int CraftingPanel::getSupplyCost(ItemID id) {
  switch (id) {
    case ItemID::I_BANDAGE: {
      return 10;
    }
    case ItemID::M_SPLINT: {
      return 30;
    }
    case ItemID::S_BANDAGE: {
      return 20;
    }
    case ItemID::S_WATER: {
      return 15;
    }
    case ItemID::P_KILLERS: {
      return 15;
    }
    case ItemID::FA_KIT: {
      return 25;
    }
    default: {
      return -1;
    }
  }
}

int CraftingPanel::getSupplyRefund(ItemID id) {
  switch (id) {
    case ItemID::I_BANDAGE: {
      return 2;
    }
    case ItemID::M_SPLINT: {
      return 6;
    }
    case ItemID::S_BANDAGE: {
      return 5;
    }
    case ItemID::S_WATER: {
      return 3;
    }
    case ItemID::P_KILLERS: {
      return 3;
    }
    case ItemID::FA_KIT: {
      return 4;
    }
    default: {
      return -1;
    }
  }
}
