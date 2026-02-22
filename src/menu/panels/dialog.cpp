#include <cmath>
#include <cassert>
#include <cstddef>
#include <string>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/panel.h"
#include "utils/input.h"
#include "utils/menu.h"
#include "menu/panels/dialog.h"
#include <plog/Log.h>

using std::string, std::vector;


DialogPanel::DialogPanel(Vector2 position, vector<string> dialog,
                         bool end_prompt, bool visible_frame, 
                         float auto_time) 
{
  id = PanelID::DIALOG;
  main_position = position;
  text_position = Vector2Add(position, {2, 1});

  texture = LoadTexture("graphics/menu/frames/dialog1.png");
  frame_height = texture.height;
  visible = visible_frame;

  menu_atlas = &Game::menu_atlas;
  menu_atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();

  keybinds = &Game::settings.menu_keybinds;

  this->dialog = dialog;
  current_line = this->dialog.begin();
  current_char = current_line->begin();

  if (end_prompt) {
    selected = prompt_options.begin();
  }

  if (auto_time >= 0) {
    this->auto_time = auto_time;
    auto_mode = true;
  }
}

DialogPanel::~DialogPanel() {
  menu_atlas->release();
  sfx->release();
  UnloadTexture(texture);
}

void DialogPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
    return;
  }

  if (current_char != current_line->end()) {
    textScrolling();
    dialog_state = DialogState::SCROLLING;
  }
  else if (current_line == dialog.end() - 1) {
    dialog_state = DialogState::END_OF_DIALOG; 
  }
  else {
    dialog_state = DialogState::END_OF_LINE;
  }

  bool gamepad = IsGamepadAvailable(0);
  if (selected != NULL && dialog_state == DialogState::END_OF_DIALOG) {
    blink_clock += Game::deltaTime();
    menuNavigation(gamepad);
  }

  if (auto_mode && dialog_state == DialogState::END_OF_LINE) {
    autoConfirm();
  }

  if (Input::pressed(keybinds->confirm, gamepad)) {
    confirm();
  }

}

void DialogPanel::menuNavigation(bool gamepad) {
  if (Input::pressed(keybinds->right, gamepad)) {
    MenuUtils::nextOption(prompt_options, selected);
    sfx->play("menu_navigate");
    blink_clock = 0.0;
  }
  else if (Input::pressed(keybinds->left, gamepad)) {
    MenuUtils::prevOption(prompt_options, selected);
    sfx->play("menu_navigate");
    blink_clock = 0.0;
  }
}

void DialogPanel::heightLerp() {
  float percentage;
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = texture.height * percentage;
}

void DialogPanel::textScrolling() {
  if (text_clock < 1.0) {
    assert(current_char != current_line->end());
    float multiplier = getSpeedMultiplier();
    text_clock += Game::deltaTime() / (text_speed * multiplier);
    return;
  }

  buffer.push_back(*current_char);
  current_char++;

  text_clock = 0.0;
}

float DialogPanel::getSpeedMultiplier() {
  if (current_char == current_line->begin()) {
    return 1.0;
  }

  string::iterator prev_char = current_char - 1;
  switch (*prev_char) {
    case ',': {
      return 3.0;
    }
    case ';': {
      return 6.0;
    }
    case '.':
    case '?':
    case '!': {
      return 8.0;
    }
    default: {
      return 1.0;
    }
  }
}

void DialogPanel::confirm() {
  switch (dialog_state) {
    case DialogState::SCROLLING: {
      PLOGI << "Skipping to the end of the line.";
      buffer = *current_line;
      current_char = current_line->end();
      break;
    }
    case DialogState::END_OF_LINE: {
      PLOGI << "Moving to the next line.";
      current_line++;
      auto_clock = 0.0;

      if (current_line != dialog.end()) {
        current_char = current_line->begin();
        buffer.clear();
        sfx->play("menu_navigate");
        break;
      }
    }
    case DialogState::END_OF_DIALOG: {
      PLOGI << "Ending Dialog.";
      state = PanelState::CLOSING;

      if (selected != NULL) {
        sfx->play("menu_select");
      }
      else {
        sfx->play("menu_cancel"); 
      }

      break;
    }
  }
}

void DialogPanel::autoConfirm() {
  assert(auto_time >= 0);
  assert(dialog_state == DialogState::END_OF_LINE);

  auto_clock += Game::deltaTime() / auto_time;
  if (auto_clock > 1.0) {
    current_line++;

    if (current_line != dialog.end()) {
      current_char = current_line->begin();
      buffer.clear();
    }

    auto_clock = 0.0;
  }
}

void DialogPanel::draw() {
  if (visible) {
    float width = texture.width;
    Rectangle source = {0, 0, width, frame_height};
    DrawTextureRec(texture, source, main_position, WHITE);
  }

  if (state != PanelState::READY) {
    return;
  }

  Font *font = &Game::med_font;
  int txt_size = font->baseSize;
  DrawTextEx(*font, buffer.c_str(), text_position, txt_size, -2, WHITE);

  if (selected != NULL && dialog_state == DialogState::END_OF_DIALOG) {
    drawOptions();
  }
}

void DialogPanel::drawOptions() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Vector2 position = Vector2Add(main_position, {56, 32});

  for (PromptOptions &option : prompt_options) {
    string text;
    if (option == PromptOptions::YES) {
      text = "YES";
    }
    else {
      text = "NO";
    }

    if (selected == &option) {
      drawCursor(position);
    }

    DrawTextEx(*font, text.c_str(), position, txt_size, -2, WHITE);
    position.x += 96;
  }
}

void DialogPanel::drawCursor(Vector2 position) {
  Color color = WHITE;

  float sin_a = std::sinf(blink_clock * 2.5);
  sin_a = (sin_a / 2) + 0.5;
  color.a = 255 * sin_a;

  position = Vector2Add(position, {-11, 2});
  DrawTextureRec(menu_atlas->sheet, menu_atlas->sprites[0], position, 
                 color);

}
