#include <algorithm>
#include <cstring>
#include <memory>
#include <cassert>
#include <cstddef>
#include <string>
#include <vector>
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/panel.h"
#include "data/session.h"
#include "utils/input.h"
#include "utils/math.h"
#include "utils/menu.h"
#include "utils/text.h"
#include "utils/items.h"
#include "system/sprite_atlas.h"
#include "scenes/camp_menu.h"
#include "menu/panels/dialog.h"
#include "menu/panels/items.h"
#include <plog/Log.h>

using std::string, std::make_unique, std::vector;
SpriteAtlas ItemsPanel::portraits("menu", "item_portraits");


ItemsPanel::ItemsPanel(Session *session, string *description, 
                       Color *desc_color) {
  id = PanelID::ITEMS;
  frame = LoadTexture("graphics/menu/frames/items.png");

  this->session = session;
  this->description = description;
  this->desc_color = desc_color;
  description->clear();

  this->keybinds = &Game::settings.menu_keybinds;

  this->sfx = &Game::menu_sfx;
  this->camp_atlas = &CampMenuScene::atlas;
  this->menu_atlas = &Game::menu_atlas;

  std::copy(session->inventory, session->inventory + 8, options.begin());
  updateSelected();

  sub_selected = sub_options.begin();

  party = {&session->player, &session->companion};
  target = party.begin();

  portraits.use();
  sfx->use();
  camp_atlas->use();
  menu_atlas->use();

  if (Game::devmode) {
    PLOGD << "Player's Inventory";
    for (int index = 0; index < session->item_limit; index++) {
      ItemID id = options.at(index);
      PLOGD << "Index " << index << ": " << static_cast<int>(id);
    }
  }
  PLOGI << "Items Panel: Initialized.";
}

ItemsPanel::~ItemsPanel() {
  std::copy(options.begin(), options.end(), session->inventory);
  *desc_color = WHITE;

  UnloadTexture(frame);
  portraits.release();
  sfx->release();
  camp_atlas->release();
  menu_atlas->release();
}

void ItemsPanel::updateSelected() {
  for (int index = 0; index < session->item_limit; index++) {
    ItemID *item = &options.at(index);

    if (*item != ItemID::NONE) {
      selected = options.begin() + index;
      swap_selected = selected;
      item_name = ItemUtils::getName(*selected);
      PLOGD << "Selected set to: " << static_cast<int>(*item);
      return;
    }
  }

  PLOGD << "Player has no items.";
  selected = NULL;
}

void ItemsPanel::resetSubSelected() {
  sub_disallowed.clear();

  assert(selected != NULL);
  if (!itemUsable(*selected)) {
    PLOGD << "Detected that the selected item is unusable.";
    sub_disallowed.emplace(ItemOptions::USE);
  }

  if (session->item_count <= 1) {
    PLOGD << "Detected that the player has one or no items.";
    sub_disallowed.emplace(ItemOptions::SWAP);
  }

  sub_selected = NULL;
  for (ItemOptions &option : sub_options) {
    if (sub_disallowed.find(option) == sub_disallowed.end()) {
      sub_selected = &option;
      break;
    }
  }

  assert(sub_selected != NULL);
}

void ItemsPanel::updateSubOptionDesc() {
  assert(sub_selected != NULL);
  switch (*sub_selected) {
    case ItemOptions::USE: {
      *description = "Use the selected item.";
      break;
    }
    case ItemOptions::SWAP: {
      *description = "Swap the selected item with another\n"
        "one.";
      break;
    }
    case ItemOptions::TOSS: {
      *description = "Discard the currently selected \n"
        "item.";
    }
  }
}

bool ItemsPanel::itemUsable(ItemID item) {
  switch (item) {
    case ItemID::I_BANDAGE:
    case ItemID::M_SPLINT:
    case ItemID::S_BANDAGE:
    case ItemID::FA_KIT: {
      return true;
    }
    default: {
      assert(item != ItemID::NONE);
      return false;
    }
  }
}

void ItemsPanel::useItem() {
  assert(option_state = TARGET);
  assert(selected != NULL);
  ItemID item = *selected;
  Character *member = *target;
  PLOGI << "Attempting to use item on: '" << member->name << "'";

  switch (item) {
    case ItemID::I_BANDAGE: {
      if (member->life >= member->max_life) {
        openRejectDialog(member);
        return;
      }

      float heal = calculateHeal(member, 0.25);
      PLOGI << "Healing combatant by: " << heal << " Life";

      member->life = Clamp(member->life + heal, 0, member->max_life);
      openHealDialog(member, heal);
      break;
    }
    case ItemID::M_SPLINT: {
      bool successful = false;
      for (int index = member->status_limit - 1; index >= 0; index--) {
        StatusID *effect = &member->status[index];

        if (*effect != StatusID::NONE) {
          PLOGI << "Curing persistant status ailment.";
          StatusID cured_effect = *effect;
          *effect = StatusID::NONE;

          member->status_count--;
          assert(member->status_count >= 0);

          openSplintDialog(member, cured_effect);
          successful = true;
          break;
        }
      }

      if (!successful) {
        openRejectDialog(member);
        return;
      }

      break;
    }
    case ItemID::S_BANDAGE: {
      if (member->life >= member->max_life) {
        openRejectDialog(member);
        return;
      }

      float heal = calculateHeal(member, 0.5);
      PLOGI << "Healing combatant by: " << heal << " Life.";

      member->life = Clamp(member->life + heal, 0, member->max_life);
      openHealDialog(member, heal);
      break;
    }
    case ItemID::FA_KIT: {
      Character *mary = *party.begin();
      Character *companion = party.at(1);

      float m_max_life = mary->max_life;
      float c_max_life = companion->max_life;

      if (mary->life == m_max_life && companion->life == c_max_life) {
        openPartyRejectDialog(companion);
        return;
      }

      float recovery = Clamp(mary->recovery, 0.0, 2.0);
      float heal = std::ceilf(6 * recovery);
      PLOGI << "Healing party by: " << heal << " Life.";

      mary->life = Clamp(mary->life + heal, 0.0, m_max_life);
      companion->life = Clamp(companion->life + heal, 0.0, c_max_life);
      openMedkitDialog(mary, companion, heal);
      break;
    }
    default: {
      assert(item != ItemID::NONE);
      return;
    }
  }

  *selected = ItemID::NONE;
  session->item_count--;
  updateSelected();
  sfx->play("menu_item");
}

void ItemsPanel::swapItems() {
  assert(selected != NULL && swap_selected != NULL);

  if (swap_selected == selected) {
    sfx->play("menu_cancel");
    return;
  }

  ItemID temp = *selected;
  *selected = *swap_selected;
  *swap_selected = temp;

  selected = swap_selected;
}

void ItemsPanel::tossItem() {
  assert(*selected != ItemID::NONE);
  *selected = ItemID::NONE;
  session->item_count--;
  updateSelected();
  sfx->play("menu_craft");
}

float ItemsPanel::calculateHeal(Character *member, float percentage) {
  float max_life = member->max_life;
  float recovery = member->recovery;

  for (int x = 0; x < STATUS_LIMIT; x++) {
    StatusID effect = member->status[x];

    if (effect == StatusID::MANGLED) {
      float resilience = member->resilience;
      float dec_percentage = Lerp(0.15, 0.85, resilience - 0.20);
      dec_percentage = Clamp(dec_percentage, 0.15, 0.85);
      recovery = recovery * dec_percentage;
      break;
    }
  }

  float multiplier = recovery * recovery;
  float result = (max_life * percentage) * multiplier;
  PLOGD << "Base Heal Amount: " << result;

  if (member->member_id != PartyMemberID::MARY) {
    Character *mary = party.at(0);
    float m_recovery = mary->recovery;
    float m_percentage = (m_recovery * m_recovery) / 6;

    float bonus = (max_life * m_percentage) * multiplier;
    PLOGD << "Bonus to be applied: " << bonus;

    result += bonus;
  }

  result = std::ceilf(result);
  return result;
}

void ItemsPanel::openDialog(vector<string> &dialog, bool prompt) {
  Vector2 position = {185, 187};
  panel = make_unique<DialogPanel>(position, dialog, prompt);
  panel_mode = true;
}

void ItemsPanel::openRejectDialog(Character *member) {
  char name[9];
  std::strcpy(name, member->name);

  const char *text = TextFormat("%s seems to be just fine.", name);
  vector<string> dialog = {text};

  openDialog(dialog);
  sfx->play("menu_cancel");
}

void ItemsPanel::openPartyRejectDialog(Character *companion) {
  assert(companion->member_id != PartyMemberID::MARY);
  char c_name[9];
  std::strcpy(c_name, companion->name);

  const char *text = TextFormat("Both Mary and %s are already\n"
                                "in perfect condition.", c_name);
  vector<string> dialog = {text};

  openDialog(dialog);
  sfx->play("menu_cancel");
}

void ItemsPanel::openHealDialog(Character *member, float healed) {
  char name[9];
  std::strcpy(name, member->name);

  vector<string> dialog;
  if (member->member_id == PartyMemberID::MARY) {
    string text = TextFormat("%s proceeds to treat his wounds.", name);
    dialog.push_back(text);
  }
  else {
    string text = TextFormat("Mary proceeds to treat %s's\n"
                             "wounds.", name);
    dialog.push_back(text);
  }

  bool maxed_out = member->life >= member->max_life;
  if (maxed_out) {
    string text = TextFormat("%s's Life was maxed out.", name);
    dialog.push_back(text);
  }
  else {
    string text = TextFormat("%s recovers %00.00f Life.", 
                                  name, healed);
    dialog.push_back(text);
  }

  if (maxed_out && member->status_count != 0) {
    string text = 
      "There's still some underlying issues\n"
      "that require proper medical attention,\n"
      "but this should be enough for now."
    ;
    dialog.push_back(text);
  }

  openDialog(dialog);
}

void ItemsPanel::openMedkitDialog(Character *mary, Character *companion,
                                  float healed)
{
  char c_name[9];
  std::strcpy(c_name, companion->name);

  vector<string> dialog;
  string text = TextFormat("Mary attempts to use a medkit to heal\n"
                           "both himself and %s.", c_name);
  dialog.push_back(text);
  text.clear();

  if (mary->life < mary->max_life) {
    text = TextFormat("Mary recovers %00.00f Life.\n", healed);
  }
  else {
    text = TextFormat("Mary's Life was maxed out.\n");
  }

  if (companion->life < companion->max_life) {
    text = text + TextFormat("%s recovers %00.00f Life.", c_name, healed);
  }
  else {
    text = text + TextFormat("%s's Life was maxed out.", c_name, healed);
  }
  dialog.push_back(text);

  openDialog(dialog);
}

void ItemsPanel::openSplintDialog(Character *member, StatusID effect) {
  vector<string> dialog;

  char name[9];
  std::strcpy(name, member->name);

  string txt_effect;
  switch (effect) {
    case StatusID::BROKEN_ARM: {
      txt_effect = "broken arm";
      break;
    }
    case StatusID::CRIPPLED_LEG: {
      txt_effect = "crippled leg";
      break;
    }
    case StatusID::MANGLED: {
      txt_effect = "mangled body";
      break;
    }
    default: {
      txt_effect = "N/A";
    }
  }

  if (member->member_id == PartyMemberID::MARY) {
    string text = TextFormat("%s carefully applies a splint to\n"
                             "his %s.",
                             name, txt_effect.c_str());
    dialog.push_back(text);
  }
  else {
    string text = TextFormat("Mary carefully applies a splint to\n"
                             "%s's %s.", name, txt_effect.c_str());
    dialog.push_back(text);
  }

  string text = TextFormat("%s's %s is now\n"
                           "properly stabilized, and fixed.", 
                           name, txt_effect.c_str());
  dialog.push_back(text);

  openDialog(dialog);
}

void ItemsPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
    return;
  }

  if (panel_mode) {
    panelLogic();
    return;
  }

  switch (option_state) {
    case OPTION: {
      optionNavigation();
      break;
    }
    case SUB_OPTION: {
      subOptionNavigation();
      break;
    }
    case TARGET: {
      targetNavigation();
      break;
    }
    case SWAP: {
      swapNavigation();
      break;
    }
  }

  blink_clock += Game::deltaTime();
}

void ItemsPanel::panelLogic() {
  assert(panel != nullptr == panel_mode);
  panel->update();

  if (!panel->terminate) {
    return;
  }

  if (panel->selected != NULL) {
    promptHandling();
  }

  panel.reset();
  panel_mode = false;
}

void ItemsPanel::promptHandling() {
  assert(*sub_selected == ItemOptions::TOSS);

  if (*panel->selected == PromptOptions::YES) {
    tossItem();
    sub_selected = sub_options.begin();
    option_state = OPTION;
    description->clear();
  }
}

void ItemsPanel::heightLerp() {
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 104 * percentage;
}

void ItemsPanel::optionNavigation() {
  bool gamepad = IsGamepadAvailable(0);
  if (selected != NULL && Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(options, selected, &disallowed);
    item_name = ItemUtils::getName(*selected);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (selected != NULL && Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected, &disallowed);
    item_name = ItemUtils::getName(*selected);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (selected != NULL && Input::pressed(keybinds->confirm, gamepad)) 
  {
    option_state = SUB_OPTION;
    resetSubSelected();
    updateSubOptionDesc();
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void ItemsPanel::swapNavigation() {
  assert(swap_selected != NULL);

  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(options, swap_selected, &disallowed);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, swap_selected, &disallowed);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    swapItems();
    option_state = OPTION;

    description->clear();
    *desc_color = WHITE;

    sfx->play("menu_select"); 
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    option_state = SUB_OPTION;

    updateSubOptionDesc();
    *desc_color = WHITE;
    sfx->play("menu_cancel");
  }
}

void ItemsPanel::subOptionNavigation() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(sub_options, sub_selected, &sub_disallowed);
    updateSubOptionDesc();
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(sub_options, sub_selected, &sub_disallowed);
    updateSubOptionDesc();
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    selectSubOption();
    blink_clock = 0.0;
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    sub_selected = sub_options.begin();
    option_state = OPTION;
    description->clear();
    sfx->play("menu_cancel");
  }
}

void ItemsPanel::selectSubOption() {
  assert(selected != NULL && *selected != ItemID::NONE);
  switch (*sub_selected) {
    case ItemOptions::USE: {
      if (*selected == ItemID::FA_KIT) {
        useItem();
        option_state = OPTION;
        description->clear();
        break;
      }

      if (itemUsable(*selected)) {
        *description = "Select a combatant to use item:\n"
              "\"" + item_name + "\"";
        *desc_color = Game::palette[22];

        target = party.begin();
        option_state = TARGET;
      }
      else {
        PLOGI << "Selected item is unusable!";
        sfx->play("menu_cancel");
      }

      break;
    }
    case ItemOptions::SWAP: {
      PLOGI << "Entering swap mode.";
      assert(session->item_count > 1);

      *description = "Select another item to swap\n"
        "\"" + item_name + "\" with.";
      *desc_color = Game::palette[22];

      option_state = SWAP;
      break;
    }
    case ItemOptions::TOSS: {
      PLOGI << "Attempting to open Toss dialog.";
      string item_name = ItemUtils::getName(*selected);
      const char *text = TextFormat("Discard \"%s\"?", item_name.c_str());
      vector<string> dialog = {text};
      openDialog(dialog, true);
    }
  }
}

void ItemsPanel::targetNavigation() {
  assert(selected != NULL);
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->right, gamepad)) {
    MenuUtils::nextOption(party, target);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->left, gamepad)) {
    MenuUtils::prevOption(party, target);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    useItem();
    option_state = OPTION;

    description->clear();
    *desc_color = WHITE;

    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    updateSubOptionDesc();
    *desc_color = WHITE;

    option_state = SUB_OPTION;
    sfx->play("menu_cancel");
  }
}

void ItemsPanel::draw() {
  Rectangle source = {0, 0, 200, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);

  drawItemCount();
  drawOptions();

  if (option_state != OPTION) {
    drawSubOptions();
  }

  if (option_state == TARGET) {
    Character *party_member = *target;
    bool leader = party_member->member_id == PartyMemberID::MARY;

    Vector2 position = {213.0f + (104.0f * !leader), 148};
    reticle.draw(position, blink_clock);
  }

  if (selected != NULL && state == PanelState::READY) {
    drawItemInfo();
  }

  if (panel_mode) {
    panel->draw();
  }
}

void ItemsPanel::drawItemCount() {
  Rectangle *connector = &camp_atlas->sprites[2];
  DrawTextureRec(camp_atlas->sheet, *connector, count_position, WHITE);

  Vector2 position = Vector2Add(count_position, {6, 0});
  Rectangle frame = camp_atlas->sprites[0];
  frame.x = Math::smoothstep(82, 38, percentage);
  frame.width = Math::smoothstep(6, 50, percentage);

  DrawTextureRec(camp_atlas->sheet, frame, position, WHITE);
  
  if (state != PanelState::READY) {
    return;
  }

  Font *font = &Game::med_font;
  int txt_size = font->baseSize;
  Color color = WHITE;

  int item_count = session->item_count;
  int item_limit = session->item_limit;

  if (item_count == 0) {
    color = Game::palette[32];
  }
  else if (item_count == item_limit) {
    color = Game::palette[29];
  }

  const char *text = TextFormat("%i / %i", item_count, item_limit);
  position = Vector2Add(position, {7, 1});
  DrawTextEx(*font, text, position, txt_size, -2, color);
}

void ItemsPanel::drawOptions() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Rectangle sprite = camp_atlas->sprites[0];
  if (state != PanelState::READY) {
    sprite.width *= percentage;
  }

  if (selected == NULL) {
    Vector2 position = option_position;
    DrawTextureRec(camp_atlas->sheet, sprite, position, WHITE);

    position = Vector2Add(position, {6, 1});
    DrawTextEx(*font, "NO ITEMS", position, txt_size, -2, WHITE);
  }

  int multiplier = 0;
  for (int index = 0; index < options.size(); index++) {
    ItemID *item = &options.at(index);

    if (*item == ItemID::NONE) {
      continue;
    }

    string name = ItemUtils::getShortened(*item);
    Vector2 position = option_position;
    position.y += 16 * multiplier;

    DrawTextureRec(camp_atlas->sheet, sprite, position, WHITE);

    if (item == selected) {
      drawCursor(position, option_state == OPTION);
    }
    else if (option_state == SWAP && item == swap_selected) {
      drawCursor(position, true);
    }

    position = Vector2Add(position, {6, 1});

    Color color = WHITE; 
    if (option_state != OPTION && item == selected) {
      color = Game::palette[22];
    }
    else if (*item == ItemID::S_WATER || *item == ItemID::P_KILLERS) {
      color = Game::palette[2];
    }

    DrawTextEx(*font, name.c_str(), position, txt_size, -2, color);
    multiplier++;
  }
}

void ItemsPanel::drawSubOptions() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Rectangle *sprite = &camp_atlas->sprites[9];
  int multiplier = 0;

  for (ItemOptions &option : sub_options) {
    string name = getSubOptionName(option);
    Vector2 position = sub_option_position;
    position.y += 16 * multiplier;

    Color color;
    Color txt_color;

    if (sub_disallowed.find(option) != sub_disallowed.end()) {
      color = Game::palette[2];
      txt_color = Game::palette[2];
    }
    else if (sub_selected == &option) {
      color = Game::palette[43];
      txt_color = WHITE;
    }
    else {
      color = Game::palette[40];
      txt_color = Game::palette[42];
    }

    DrawTextureRec(camp_atlas->sheet, *sprite, position, color);
    position = Vector2Add(position, {7, 1});

    DrawTextEx(*font, name.c_str(), position, txt_size, -2, txt_color);
    multiplier++;
  }
}

string ItemsPanel::getSubOptionName(ItemOptions option) {
  switch (option) {
    case ItemOptions::USE: {
      return "USE";
    }
    case ItemOptions::SWAP: {
      return "SWAP";
    }
    case ItemOptions::TOSS: {
      return "TOSS";
    }
  }
}

void ItemsPanel::drawCursor(Vector2 position, bool blink) {
  if (state != PanelState::READY) {
    return;
  }

  Rectangle *sprite = &menu_atlas->sprites[0];
  position = Vector2Add(position, {-13, 3});

  Color color = WHITE;

  if (blink) {
    float sin_a = std::sinf(blink_clock * 2.5);
    sin_a = (sin_a / 2) + 0.5;
    color.a = 255 * sin_a;
  }
  else {
    color = Game::palette[22];
  }

  DrawTextureRec(menu_atlas->sheet, *sprite, position, color);
}

void ItemsPanel::drawItemInfo() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  drawItemName(font, txt_size);
  drawItemType(font, txt_size);
  drawItemUsable(font, txt_size);
  drawItemDesc(font, txt_size);
  drawItemPortrait();
}

void ItemsPanel::drawItemName(Font *font, int txt_size) {
  Vector2 position = Vector2Add(frame_position, {42, 1});
  Color color = Game::palette[22];

  DrawTextEx(*font, item_name.c_str(), position, txt_size, -2, color);
}

void ItemsPanel::drawItemType(Font *font, int txt_size) {
  string type;
  switch (*selected) {
    case ItemID::I_BANDAGE:
    case ItemID::S_BANDAGE:
    case ItemID::FA_KIT: {
      type = "Restorative Item";
      break;
    }
    case ItemID::M_SPLINT: {
      type = "Curative Item";
      break;
    }
    case ItemID::S_WATER: {
      type = "Enhancement Item";
      break;
    }
    case ItemID::P_KILLERS: {
      type = "Analgesic Item";
      break;
    }
    default: {
      assert(*selected != ItemID::NONE);
    }
  }

  Vector2 position = Vector2Add(frame_position, {196, 14});
  position = TextUtils::alignRight(type.c_str(), position, *font, -2, 0);

  DrawTextEx(*font, type.c_str(), position, txt_size, -2, WHITE);
}

void ItemsPanel::drawItemUsable(Font *font, int txt_size) {
  string usable;
  switch (*selected) {
    case ItemID::I_BANDAGE:
    case ItemID::M_SPLINT: 
    case ItemID::S_BANDAGE: 
    case ItemID::FA_KIT: {
      usable = "Always";
      break;
    }
    case ItemID::S_WATER: 
    case ItemID::P_KILLERS :{
      usable = "In Combat";
      break;
    }
    default: {
      assert(*selected != ItemID::NONE);
    }
  }

  Vector2 position = Vector2Add(frame_position, {196, 27});
  position = TextUtils::alignRight(usable.c_str(), position, *font, -2, 
                                   0);

  DrawTextEx(*font, usable.c_str(), position, txt_size, -2, WHITE);
}

void ItemsPanel::drawItemDesc(Font *font, int txt_size) {
  string desc = ItemUtils::getDescription(*selected);
  Vector2 position = Vector2Add(frame_position, {2, 40});
  DrawTextEx(*font, desc.c_str(), position, txt_size, -2, WHITE);
}

void ItemsPanel::drawItemPortrait() {
  assert(*selected != ItemID::NONE);
  int id = static_cast<int>(*selected);
  Rectangle *sprite = &portraits.sprites.at(id);
  Vector2 position = Vector2Add(frame_position, {5, 5});

  DrawTextureRec(portraits.sheet, *sprite, position, WHITE);
}
