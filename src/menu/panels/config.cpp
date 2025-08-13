#include <cassert>
#include <cmath>
#include <utility>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/keybinds.h"
#include "system/sprite_atlas.h"
#include "utils/input.h"
#include "menu/panels/config.h"
#include <plog/Log.h>

using std::pair, std::make_pair, std::string;


ConfigPanel::ConfigPanel(Vector2 position, SpriteAtlas *menu_atlas,
                         MenuKeybinds *menu_keybinds) 
{
  id = PanelID::CONFIG;
  this->position = position;

  frame = LoadTexture("graphics/menu/config_frame.png");
  setupOptions();
  selected = options.begin();

  keybinds = menu_keybinds;

  assert(menu_atlas != NULL);
  atlas = menu_atlas;
  atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();

  on_linux = PLATFORM == PLATFORM_LINUX;
  PLOGI << "Configuration Panel: Initialized.";
}

void ConfigPanel::setupOptions() {
  options = {
    make_pair(ConfigOption::VOL_MASTER, 55),
    make_pair(ConfigOption::VOL_SOUND, 71),
    make_pair(ConfigOption::VOL_MUSIC, 87),
    make_pair(ConfigOption::FULLSCREEN, 111),
    make_pair(ConfigOption::FRAMERATE, 127),
    make_pair(ConfigOption::CONTROLS, 151),
    make_pair(ConfigOption::APPLY, 167),
    make_pair(ConfigOption::BACK, 183)
  };
}

ConfigPanel::~ConfigPanel() {
  UnloadTexture(frame);
  atlas->release();
  sfx->release();
}

void ConfigPanel::update() {
  if (state != READY) {
    frameTransition();
    return;
  }

  blink_clock += Game::deltaTime();
  optionNavigation();
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

void ConfigPanel::optionNavigation() {
  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->down, gamepad)) {
    nextOption();
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    prevOption();
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  if (Input::pressed(keybinds->cancel, gamepad)) {
    state = CLOSING;
  }
}

void ConfigPanel::nextOption() {
  selected++;

  if (selected == options.end()) {
    selected = options.begin();
  }

  if (on_linux && selected->first == ConfigOption::FULLSCREEN) {
    selected++;
  }
}

void ConfigPanel::prevOption() {
  if (selected == options.begin()) {
    selected = options.end();
  }

  selected--;

  if (on_linux && selected->first == ConfigOption::FULLSCREEN) {
    selected--;
  }
}

void ConfigPanel::draw() {
  Rectangle source = {0, 0, 232, frame_height};
  DrawTextureRec(frame, source, position, WHITE);

  if (state == READY) {
    drawOptions();
  }
}

void ConfigPanel::drawOptions() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  for (pair<ConfigOption, float> option : options) {
    string name = getOptionName(option.first);
    Vector2 position = {113, option.second};

    if (option == *selected) {
      drawCursor(position);
    }

    if (!on_linux || option.first != ConfigOption::FULLSCREEN) {
      DrawTextEx(*font, name.c_str(), position, txt_size, -2, WHITE);
    }
  }
}

void ConfigPanel::drawCursor(Vector2 position) {
  Color color = WHITE;

  float sin_a = std::sinf(blink_clock * 2.5);
  sin_a = (sin_a / 2) + 0.5;
  color.a = 255 * sin_a;

  position = Vector2Add(position, {-11, 2});
  DrawTextureRec(atlas->sheet, atlas->sprites[0], position, color);
}

string ConfigPanel::getOptionName(ConfigOption id) {
  switch (id) {
    case ConfigOption::VOL_MASTER: {
      return "Master Volume";
    }
    case ConfigOption::VOL_SOUND: {
      return "Sound Volume";
    }
    case ConfigOption::VOL_MUSIC: {
      return "Music Volume";
    }
    case ConfigOption::FULLSCREEN: {
      return "Fullscreen";
    }
    case ConfigOption::FRAMERATE: {
      return "Framerate";
    }
    case ConfigOption::CONTROLS: {
      return "CONTROLS";
    }
    case ConfigOption::APPLY: {
      return "APPLY SETTINGS";
    }
    case ConfigOption::BACK: {
      return "BACK";
    }
  }
}

