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
#include "scenes/camp_menu.h"
#include "menu/panels/items.h"
#include <plog/Log.h>

using std::string;


ItemsPanel::ItemsPanel(Session *session, string *description) {
  id = PanelID::ITEMS;
  frame = LoadTexture("graphics/menu/items_frame.png");

  this->session = session;
  this->description = description;

  this->keybinds = &Game::settings.menu_keybinds;

  this->sfx = &Game::menu_sfx;
  this->camp_atlas = &CampMenuScene::atlas;
  this->menu_atlas = &CampMenuScene::menu_atlas;

  std::copy(session->inventory, session->inventory + 8, options.begin());
  updateSelected();

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
  UnloadTexture(frame);
  sfx->release();
  camp_atlas->release();
  menu_atlas->release();
}

void ItemsPanel::updateSelected() {
  for (int index = 0; index < session->item_limit; index++) {
    ItemID *item = &options.at(index);

    if (*item != ItemID::NONE) {
      selected = options.begin() + index;
      PLOGD << "Selected set to: " << static_cast<int>(*item);
      return;
    }
  }

  PLOGD << "Player has no items.";
  selected = NULL;
}

void ItemsPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
    return;
  }

  optionNavigation();
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
    blink_clock = 0.0;
  }
  else if (selected != NULL && Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected, &disallowed);
    sfx->play("menu_navigate");
    blink_clock = 0.0;
  }
  if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void ItemsPanel::draw() {
  Rectangle source = {0, 0, 200, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);

  drawItemCount();
  drawOptions();
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

string ItemsPanel::getShortenedName(ItemID item) {
  switch (item) {
    case ItemID::I_BANDAGE: {
      return "I.Bandage";
    }
    case ItemID::M_SPLINT: {
      return "M.Splint";
    }
    default: {
      return "N / A";
    }
  }
}

void ItemsPanel::drawCursor(Vector2 position) {
  if (state != PanelState::READY) {
    return;
  }

  Rectangle *sprite = &menu_atlas->sprites[0];
  position = Vector2Add(position, {-13, 3});

  Color color = WHITE;
  float sin_a = std::sinf(blink_clock * 2.5);
  sin_a = (sin_a / 2) + 0.5;
  color.a = 255 * sin_a;

  DrawTextureRec(menu_atlas->sheet, *sprite, position, color);
}
