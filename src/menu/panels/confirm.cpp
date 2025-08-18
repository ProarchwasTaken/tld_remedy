#include <cassert>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/keybinds.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
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
}
