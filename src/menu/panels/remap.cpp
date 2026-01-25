#include <cmath>
#include <cassert>
#include <cstddef>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/panel.h"
#include "data/personal.h"
#include "data/keybinds.h"
#include "utils/input.h"
#include "utils/text.h"
#include "utils/menu.h"
#include "system/sprite_atlas.h"
#include "menu/panels/remap.h"
#include <plog/Log.h>

using std::string;


RemapPanel::RemapPanel(SpriteAtlas *menu_atlas, Settings *settings) {
  id = PanelID::REMAP;

  frame = LoadTexture("graphics/menu/remap_frame.png");

  atlas = menu_atlas;
  atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();

  this->settings = settings; 
  this->keybinds = &Game::settings.menu_keybinds;

  current_category = category.begin();
  updateDisallowed();
  updateSelected();
  PLOGI << "Control Remapping Panel: Initialized";
}

RemapPanel::~RemapPanel() {
  UnloadTexture(frame);
  atlas->release();
  sfx->release();
}

void RemapPanel::updateDisallowed() {
  PLOGI << "Updating disallowed options.";
  switch (*current_category) {
    case InputCategory::FIELD: {
      disallowed = {
        RemapOption::INPUT_X,
        RemapOption::INPUT_Y,
        RemapOption::INPUT_L,
        RemapOption::INPUT_R,
        RemapOption::INPUT_START,
        RemapOption::INPUT_SELECT
      };
      break;
    }
    case InputCategory::COMBAT: {
      disallowed = {RemapOption::INPUT_UP};
      break;
    }
  }
}

void RemapPanel::updateSelected() {
  selected = NULL;

  for (RemapOption &option : options) {
    if (disallowed.find(option) == disallowed.end()) {
      selected = &option;
      break;
    }
  }

  assert(selected != NULL);
}

KeyBind *RemapPanel::retrieveFieldKeybind(RemapOption option) {
  FieldKeybinds *keybinds = &settings->field_keybinds;

  switch (option) {
    case RemapOption::INPUT_RIGHT: {
      return &keybinds->move_right;
    }
    case RemapOption::INPUT_LEFT: {
      return &keybinds->move_left;
    }
    case RemapOption::INPUT_DOWN: {
      return &keybinds->move_down;
    }
    case RemapOption::INPUT_UP: {
      return &keybinds->move_up;
    }
    case RemapOption::INPUT_A: {
      return &keybinds->interact;
    }
    case RemapOption::INPUT_B: {
      return &keybinds->open_menu;
    }
    default: {
      return NULL;
    } 
  }
}

KeyBind *RemapPanel::retrieveCombatKeyBind(RemapOption option) {
  CombatKeybinds *keybinds = &settings->combat_keybinds;

  switch (option) {
    case RemapOption::INPUT_RIGHT: {
      return &keybinds->move_right;
    }
    case RemapOption::INPUT_LEFT: {
      return &keybinds->move_left;
    }
    case RemapOption::INPUT_DOWN: {
      return &keybinds->down;
    }
    case RemapOption::INPUT_A: {
      return &keybinds->attack;
    }
    case RemapOption::INPUT_B: {
      return &keybinds->defensive;
    }
    case RemapOption::INPUT_X: {
      return &keybinds->light_tech;
    }
    case RemapOption::INPUT_Y: {
      return &keybinds->heavy_tech;
    }
    case RemapOption::INPUT_L: {
      return &keybinds->light_assist;
    }
    case RemapOption::INPUT_R: {
      return &keybinds->heavy_assist;
    }
    case RemapOption::INPUT_START: {
      return &keybinds->pause;
    }
    case RemapOption::INPUT_SELECT: {
      return &keybinds->use_item;
    }
    default: {
      return NULL;
    } 
  }
}

void RemapPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
    return;
  }

  optionNavigation();
  blink_clock += Game::deltaTime();
}

void RemapPanel::heightLerp() {
  float percentage;
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 231 * percentage;
}

void RemapPanel::optionNavigation() {
  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->right, gamepad)) {
    MenuUtils::nextOption(category, current_category);
    updateDisallowed();
    updateSelected();

    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->left, gamepad)) {
    MenuUtils::prevOption(category, current_category);
    updateDisallowed();
    updateSelected();

    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(options, selected, &disallowed);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected, &disallowed);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void RemapPanel::draw() {
  Rectangle source = {0, 0, 306, frame_height};
  DrawTextureRec(frame, source, main_position, WHITE);

  if (state == PanelState::READY) {
    Font *font = &Game::med_font;
    int txt_size = font->baseSize;

    drawCategory(font, txt_size);
    drawOptions(font, txt_size);
  }
}

void RemapPanel::drawCategory(Font *font, int txt_size) {
  string text;
  switch (*current_category) {
    case InputCategory::FIELD: {
      text = "Field";
      break;
    }
    case InputCategory::COMBAT: {
      text = "Combat";
      break;
    }
  }

  Vector2 position = {270, 5};
  position = TextUtils::alignCenter(text.c_str(), position, *font, -2, 0);
  DrawTextEx(*font, text.c_str(), position, txt_size, -2, WHITE);
}

void RemapPanel::drawOptions(Font *font, int txt_size) {
  Vector2 position = {76, 39};

  for (RemapOption &option : options) {
    if (disallowed.find(option) != disallowed.end()) {
      continue;
    }

    if (selected == &option) {
      drawCursor(position);
    }

    string text;
    if (*current_category == InputCategory::FIELD) {
      text = getFieldKeybindName(option);
    }
    else {
      text = getCombatKeyBindName(option);
    }

    drawKeybind(option, position, font, txt_size);


    DrawTextEx(*font, text.c_str(), position, txt_size, -2, WHITE);
    position.y += 16;
  }
}

void RemapPanel::drawCursor(Vector2 position) {
  Color color = WHITE;
  float sin_a = std::sinf(blink_clock * 2.5);
  sin_a = (sin_a / 2) + 0.5;
  color.a = 255 * sin_a;

  position = Vector2Add(position, {-11, 2});
  DrawTextureRec(atlas->sheet, atlas->sprites[0], position, color);
}

void RemapPanel::drawKeybind(RemapOption &option, Vector2 position,
                             Font *font, int txt_size) 
{
  KeyBind *keybind;
  if (*current_category == InputCategory::FIELD) {
    keybind = retrieveFieldKeybind(option);
  }
  else {
    keybind = retrieveCombatKeyBind(option);
  }

  assert(keybind != NULL);
  string key_name = getKeyName(keybind->key);
  
  position.x += 155;
  position = TextUtils::alignCenter(key_name.c_str(), position, *font, -2, 
                                    0);
  DrawTextEx(*font, key_name.c_str(), position, txt_size, -2, WHITE);
}

string RemapPanel::getKeyName(KeyboardKey key) {
  string name;

  switch (key) {
    default: {
      char letter = key;
      name.push_back(letter);
    }
  }

  return name;
}

string RemapPanel::getFieldKeybindName(RemapOption option) {
  switch (option) {
    case RemapOption::INPUT_RIGHT: {
      return "Move Right";
    }
    case RemapOption::INPUT_LEFT: {
      return "Move Left";
    }
    case RemapOption::INPUT_DOWN: {
      return "Move Down";
    }
    case RemapOption::INPUT_UP: {
      return "Move Up";
    }
    case RemapOption::INPUT_A: {
      return "Interact";
    }
    case RemapOption::INPUT_B: {
      return "Open Menu";
    }
    default: {
      return "N / A";
    }
  }
}

string RemapPanel::getCombatKeyBindName(RemapOption option) {
  switch (option) {
    case RemapOption::INPUT_RIGHT: {
      return "Move Right";
    }
    case RemapOption::INPUT_LEFT: {
      return "Move Left";
    }
    case RemapOption::INPUT_DOWN: {
      return "Down";
    }
    case RemapOption::INPUT_A: {
      return "Attack";
    }
    case RemapOption::INPUT_B: {
      return "Defensive";
    }
    case RemapOption::INPUT_X: {
      return "Light Technique";
    }
    case RemapOption::INPUT_Y: {
      return "Heavy Technique";
    }
    case RemapOption::INPUT_L: {
      return "Light Assist";
    }
    case RemapOption::INPUT_R: {
      return "Heavy Assist";
    }
    case RemapOption::INPUT_START: {
      return "Pause";
    }
    case RemapOption::INPUT_SELECT: {
      return "Open Item Menu";
    }
    default: {
      return "N / A";
    }
  }
}

