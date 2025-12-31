#include <cassert>
#include <string>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/panel.h"
#include "utils/input.h"
#include "menu/panels/dialog.h"
#include <plog/Log.h>

using std::string, std::vector;


DialogPanel::DialogPanel(Vector2 position, vector<string> dialog,
                         string scroll_sound) {
  id = PanelID::DIALOG;
  main_position = position;
  text_position = Vector2Add(position, {2, 1});

  texture = LoadTexture("graphics/menu/dialog_frame1.png");
  frame_height = texture.height;

  sfx = &Game::menu_sfx;
  sfx->use();
  this->scroll_sound = scroll_sound;

  this->dialog = dialog;
  current_line = this->dialog.begin();
  current_char = current_line->begin();

  keybinds = &Game::settings.menu_keybinds;
}

DialogPanel::~DialogPanel() {
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
  else if (current_line != dialog.end()) {
    dialog_state = DialogState::END_OF_LINE;
  }
  else {
    dialog_state = DialogState::END_OF_DIALOG;
  }

  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->confirm, gamepad)) {
    confirm();
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

  sfx->play(scroll_sound, 1.30, true);
  text_clock = 0.0;
}

float DialogPanel::getSpeedMultiplier() {
  if (current_char == current_line->begin()) {
    return 1.0;
  }

  string::iterator prev_char = current_char - 1;
  switch (*prev_char) {
    case ',':
    case ';': {
      return 3.0;
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

      if (current_line != dialog.end()) {
        current_char = current_line->begin();
        buffer.clear();
        break;
      }
    }
    case DialogState::END_OF_DIALOG: {
      PLOGI << "Ending Dialog.";
      state = PanelState::CLOSING;
      break;
    }
  }
}

void DialogPanel::draw() {
  float width = texture.width;
  Rectangle source = {0, 0, width, frame_height};
  DrawTextureRec(texture, source, main_position, WHITE);

  if (state == PanelState::READY) {
    Font *font = &Game::med_font;
    int txt_size = font->baseSize;
    DrawTextEx(*font, buffer.c_str(), text_position, txt_size, -2, WHITE);
  }
}
