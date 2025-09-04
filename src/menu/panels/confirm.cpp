#include <cassert>
#include <string>
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/keybinds.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "utils/text.h"
#include "utils/menu.h"
#include "utils/input.h"
#include "menu/panels/confirm.h"
#include <plog/Log.h>

using std::string;


ConfirmPanel::ConfirmPanel(SpriteAtlas *menu_atlas, 
                           MenuKeybinds *menu_keybinds,
                           string message,
                           string txt_yes,
                           string txt_no)
{
  id = PanelID::CONFIRM;

  this->message = message;
  this->txt_yes = txt_yes;
  this->txt_no = txt_no;

  frame = LoadTexture("graphics/menu/confirm_frame.png");
  selected = options.begin();

  keybinds = menu_keybinds;
  atlas = menu_atlas;
  atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();
  PLOGI << "Confirmation Panel: Initializated.";
}

ConfirmPanel::~ConfirmPanel() {
  UnloadTexture(frame);
  atlas->release();
  sfx->release();
}

void ConfirmPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
    return;
  }

  blink_clock += Game::deltaTime();

  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->right, gamepad)) {
    MenuUtils::nextOption(options, selected);
    sfx->play("menu_navigate");
    blink_clock = 0.0;
  }
  else if (Input::pressed(keybinds->left, gamepad)) {
    MenuUtils::prevOption(options, selected);
    sfx->play("menu_navigate");
    blink_clock = 0.0;
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    sfx->play("menu_select");
    state = PanelState::CLOSING;
  }
}

void ConfirmPanel::heightLerp() {
  float percentage;
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 80 * percentage;
}

void ConfirmPanel::draw() {
  Rectangle source = {0, 0, 208, frame_height};
  DrawTextureRec(frame, source, position, WHITE);

  if (state == PanelState::READY) {
    Font *font = &Game::med_font;
    int txt_size = font->baseSize;

    drawMessage(font, txt_size, {213, 96});
    drawOptions(font, txt_size);
  }
}

void ConfirmPanel::drawMessage(Font *font, int txt_size, 
                               Vector2 position)
{
  position = TextUtils::alignCenter(message.c_str(), position, *font, -2, 
                                    0);
  DrawTextEx(*font, message.c_str(), position, txt_size, -2, WHITE);

}

void ConfirmPanel::drawOptions(Font *font, int txt_size) {
  Vector2 pos1 = {160, 139};
  Vector2 pos2 = {264, 139};

  pos1 = TextUtils::alignCenter(txt_yes.c_str(), pos1, *font, -2, 0);
  pos2 = TextUtils::alignCenter(txt_no.c_str(), pos2, *font, -2, 0);

  DrawTextEx(*font, txt_yes.c_str(), pos1, txt_size, -2, WHITE);
  DrawTextEx(*font, txt_no.c_str(), pos2, txt_size, -3, WHITE);

  if (selected == options.begin()) {
    drawCursor(pos1);
  }
  else {
    drawCursor(pos2);
  }
}

void ConfirmPanel::drawCursor(Vector2 position) {
  Color color = WHITE;

  float sin_a = std::sinf(blink_clock * 2.5);
  sin_a = (sin_a / 2) + 0.5;
  color.a = 255 * sin_a;

  position = Vector2Add(position, {-11, 2});
  DrawTextureRec(atlas->sheet, atlas->sprites[0], position, color);
}

