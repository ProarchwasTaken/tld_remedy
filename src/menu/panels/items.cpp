#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <string>
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
#include "system/sprite_atlas.h"
#include "scenes/camp_menu.h"
#include "menu/panels/items.h"
#include <plog/Log.h>

using std::string;
SpriteAtlas ItemsPanel::portraits("menu", "item_portraits");


ItemsPanel::ItemsPanel(Session *session, string *description, 
                       Color *desc_color) {
  id = PanelID::ITEMS;
  frame = LoadTexture("graphics/menu/items_frame.png");

  this->session = session;
  this->description = description;
  description->clear();

  this->desc_color = desc_color;
  *desc_color = Game::palette[22];

  this->keybinds = &Game::settings.menu_keybinds;

  this->sfx = &Game::menu_sfx;
  this->camp_atlas = &CampMenuScene::atlas;
  this->menu_atlas = &CampMenuScene::menu_atlas;

  std::copy(session->inventory, session->inventory + 8, options.begin());
  updateSelected();

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
      item_name = getName(*selected);
      PLOGD << "Selected set to: " << static_cast<int>(*item);
      return;
    }
  }

  PLOGD << "Player has no items.";
  selected = NULL;
}

string ItemsPanel::getName(ItemID item) {
  switch (item) {
    case ItemID::I_BANDAGE: {
      return "Improvised Bandage";
    }
    case ItemID::M_SPLINT: {
      return "Makeshift Splint";
    }
    default: {
      assert(item != ItemID::NONE);
      return "";
    }
  }
}

string ItemsPanel::getShortenedName(ItemID item) {
  switch (item) {
    case ItemID::I_BANDAGE: {
      return "I.Bandage";
    }
    case ItemID::M_SPLINT: {
      return "M.Splint";
    }
    default: {
      assert(item != ItemID::NONE);
      return "N / A";
    }
  }
}

string ItemsPanel::getDescription(ItemID item) {
  switch (item) {
    case ItemID::I_BANDAGE: {
      return 
      "Restores 35% of a Combatant's\n"
      "Life.\n"
      "In Combat: Instead applies the\n"
      "\"Mending\" status effect.";
    }
    case ItemID::M_SPLINT: {
      return
      "Can cure \"Broken Arm\",\n"
      "\"Crippled Leg\", and \"Mangled\".\n"
      "In Combat: Also cures the\n"
      "\"Despondent\" status ailment.";
    }
    default: {
      assert(item != ItemID::NONE);
      return "DESCRIPTION NOT\nAVAILIABLE";
    }
  }
}

void ItemsPanel::useItem() {
  assert(target_mode);
  assert(selected != NULL);
  ItemID item = *selected;
  Character *member = *target;
  PLOGI << "Attempting to use item on: '" << member->name << "'";

  switch (item) {
    case ItemID::I_BANDAGE: {
      float heal = std::ceilf(member->max_life * 0.35);
      PLOGI << "Healing combatant by: " << heal << " Life";

      member->life = Clamp(member->life + heal, 0, member->max_life);
      break;
    }
    case ItemID::M_SPLINT: {
      for (int index = member->status_limit - 1; index >= 0; index--) {
        StatusID *effect = &member->status[index];

        if (*effect != StatusID::NONE) {
          PLOGI << "Curing persistant status ailment.";
          *effect = StatusID::NONE;
          member->status_count--;
          assert(member->status_count >= 0);
          break;
        }
      }

      break;
    }
    default: {
      assert(item != ItemID::NONE);
    }
  }

  *selected = ItemID::NONE;
  session->item_count--;
  updateSelected();
  sfx->play("menu_item");
}

void ItemsPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
    return;
  }

  if (!target_mode) {
    optionNavigation();
  }
  else {
    targetNavigation();
  }

  blink_clock += Game::deltaTime();
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
    sfx->play("menu_navigate");
    item_name = getName(*selected);
    blink_clock = 0.0;
  }
  else if (selected != NULL && Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected, &disallowed);
    sfx->play("menu_navigate");
    item_name = getName(*selected);
    blink_clock = 0.0;
  }
  else if (selected != NULL && Input::pressed(keybinds->confirm, gamepad)) 
  {
    *description = "Select a combatant to use item:\n"
      "\"" + item_name + "\"";
    target = party.begin();
    sfx->play("menu_select");
    target_mode = true;
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void ItemsPanel::targetNavigation() {
  assert(selected != NULL);
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->right, gamepad)) {
    MenuUtils::nextOption(party, target);
    sfx->play("menu_navigate");
    blink_clock = 0.0;
  }
  else if (Input::pressed(keybinds->left, gamepad)) {
    MenuUtils::prevOption(party, target);
    sfx->play("menu_navigate");
    blink_clock = 0.0;
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    useItem();
    target_mode = false;
    *description = "";
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    target_mode = false;
    *description = "";
    sfx->play("menu_cancel");
  }
}

void ItemsPanel::draw() {
  Rectangle source = {0, 0, 200, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);

  drawItemCount();
  drawOptions();

  if (selected != NULL && state == PanelState::READY) {
    drawItemInfo();
  }

  if (target_mode) {
    Character *party_member = *target;
    bool leader = party_member->member_id == PartyMemberID::MARY;

    Vector2 position = {213.0f + (104.0f * !leader), 148};
    reticle.draw(position, blink_clock);
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

    string name = getShortenedName(*item);
    Vector2 position = option_position;
    position.y += 16 * multiplier;

    DrawTextureRec(camp_atlas->sheet, sprite, position, WHITE);

    if (options.begin() + index == selected) {
      drawCursor(position);
    }

    position = Vector2Add(position, {6, 1});
    DrawTextEx(*font, name.c_str(), position, txt_size, -2, WHITE);

    multiplier += 1;
  }
}

void ItemsPanel::drawCursor(Vector2 position) {
  if (state != PanelState::READY) {
    return;
  }

  Rectangle *sprite = &menu_atlas->sprites[0];
  position = Vector2Add(position, {-13, 3});

  Color color = WHITE;

  if (!target_mode) {
    float sin_a = std::sinf(blink_clock * 2.5);
    sin_a = (sin_a / 2) + 0.5;
    color.a = 255 * sin_a;
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
    case ItemID::I_BANDAGE: {
      type = "Restorative Item";
      break;
    }
    case ItemID::M_SPLINT: {
      type = "Curative Item";
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
    case ItemID::M_SPLINT: {
      usable = "Always";
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
  string desc = getDescription(*selected);
  Vector2 position = Vector2Add(frame_position, {2, 40});
  DrawTextEx(*font, desc.c_str(), position, txt_size, -2, WHITE);
}

void ItemsPanel::drawItemPortrait() {
  assert(*selected != ItemID::NONE);
  int id = static_cast<int>(*selected);
  Rectangle *sprite = &portraits.sprites[id];
  Vector2 position = Vector2Add(frame_position, {5, 5});

  DrawTextureRec(portraits.sheet, *sprite, position, WHITE);
}
