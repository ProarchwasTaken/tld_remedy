#include <cassert>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/keybinds.h"
#include "system/sprite_atlas.h"
#include "utils/input.h"
#include "menu/panels/config.h"
#include <plog/Log.h>


ConfigPanel::ConfigPanel(Vector2 position, SpriteAtlas *menu_atlas,
                         MenuKeybinds *menu_keybinds) 
{
  id = PanelID::CONFIG;
  this->position = position;

  frame = LoadTexture("graphics/menu/config_frame.png");

  keybinds = menu_keybinds;

  assert(menu_atlas != NULL);
  atlas = menu_atlas;
  atlas->use();
  PLOGI << "Configuration Panel: Initialized.";
}

ConfigPanel::~ConfigPanel() {
  UnloadTexture(frame);
  atlas->release();
}

void ConfigPanel::update() {
  if (state != READY) {
    frameTransition();
    return;
  }

  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->cancel, gamepad)) {
    state = CLOSING;
  }
}

void ConfigPanel::frameTransition() {
  assert(state != READY);
  clock += Game::deltaTime() / transition_time;
  clock = Clamp(clock, 0.0, 1.0);

  float percentage;
  if (state == OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 161 * percentage;

  if (clock != 1.0) {
    return;
  }

  if (state == OPENING) {
    state = READY;
    clock = 0.0;
  }
  else {
    terminate = true;
  }
}

void ConfigPanel::draw() {
  Rectangle source = {0, 0, 232, frame_height};
  DrawTextureRec(frame, source, position, WHITE);
}

