#include <cassert>
#include <cmath>
#include <cstddef>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/keybinds.h"
#include "system/sprite_atlas.h"
#include "utils/input.h"
#include "utils/text.h"
#include "utils/menu.h"
#include "menu/panels/config.h"
#include <plog/Log.h>

using std::string;


ConfigPanel::ConfigPanel(SpriteAtlas *menu_atlas,
                         MenuKeybinds *menu_keybinds) 
{
  id = PanelID::CONFIG;
  this->position = {97, 39};

  frame = LoadTexture("graphics/menu/config_frame.png");
  selected = options.begin();

  settings = Game::settings;
  keybinds = menu_keybinds;

  assert(menu_atlas != NULL);
  atlas = menu_atlas;
  atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();

  on_linux = PLATFORM == PLATFORM_LINUX;
  if (on_linux) {
    disallowed.emplace(ConfigOption::FULLSCREEN);
  }
  PLOGI << "Configuration Panel: Initialized.";
}

ConfigPanel::~ConfigPanel() {
  UnloadTexture(frame);
  atlas->release();
  sfx->release();
}

void ConfigPanel::update() {
  if (state != PanelState::READY) {
    frameTransition();
    return;
  }

  blink_clock += Game::deltaTime();

  bool gamepad = IsGamepadAvailable(0);
  verticalNavigation(gamepad);
  horizontalInput(gamepad);
}

void ConfigPanel::frameTransition() {
  assert(state != PanelState::READY);
  clock += Game::deltaTime() / transition_time;
  clock = Clamp(clock, 0.0, 1.0);

  float percentage;
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 161 * percentage;

  if (clock != 1.0) {
    return;
  }

  if (state == PanelState::OPENING) {
    state = PanelState::READY;
    clock = 0.0;
  }
  else {
    terminate = true;
  }
}

void ConfigPanel::verticalNavigation(bool gamepad) {
  if (Input::pressed(keybinds->down, gamepad)) {
    MenuUtils::nextOption(options, selected, disallowed);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->up, gamepad)) {
    MenuUtils::prevOption(options, selected, disallowed);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    selectOption();
  } 
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    sfx->play("menu_cancel");
    state = PanelState::CLOSING;
  }
}

void ConfigPanel::horizontalInput(bool gamepad) {
  bool right = Input::pressed(keybinds->right, gamepad);
  bool left = Input::pressed(keybinds->left, gamepad);

  int direction = right - left;
  if (direction == 0) {
    return;
  }

  bool changed = false;
  switch (*selected) {
    case ConfigOption::VOL_MASTER: {
      changed = changeVolume(direction, &settings.master_volume);
      break;
    }
    case ConfigOption::VOL_SOUND: {
      changed = changeVolume(direction, &settings.sfx_volume);
      break;
    }
    case ConfigOption::VOL_MUSIC: {
      changed = changeVolume(direction, &settings.bgm_volume);
      break;
    }
    case ConfigOption::FRAMERATE: {
      changed = changeFramerate(direction);
      break;
    }
    default: {
      
    }
  }

  if (changed) {
    unapplied = settings != Game::settings;
    return;
  }
}

bool ConfigPanel::changeVolume(int direction, float *percentage) {
  assert(percentage != NULL);
  float old = *percentage;
  int segments = *percentage * 20;
  segments = Clamp(segments + direction, 0, 20);

  *percentage = segments / 20.0f;

  PLOGD << "Changed volume percentage to: " << *percentage;
  sfx->play("menu_navigate");
  return *percentage != old;
}

bool ConfigPanel::changeFramerate(int direction) {
  int *framerate = &settings.framerate;
  int old = *framerate;

  *framerate = Clamp(*framerate + (5 * direction), 30, 300);
  
  PLOGD << "Changed Framerate to: " << *framerate;
  sfx->play("menu_navigate");
  return *framerate != old;
}

void ConfigPanel::selectOption() {
  switch (*selected) {
    case ConfigOption::FULLSCREEN: {
      settings.fullscreen = !settings.fullscreen;
      unapplied = settings != Game::settings;
      sfx->play("menu_select");
      break;
    }
    case ConfigOption::APPLY: {
      applySettings();
      sfx->play("menu_select");
      break;
    }
    case ConfigOption::BACK: {
      state = PanelState::CLOSING;
      sfx->play("menu_cancel");
      break;
    }
    case ConfigOption::CONTROLS: {
      PLOGE << "Controls Remapping hasn't been implemented yet!";
    }
    default: {
      sfx->play("menu_cancel");
    }
  }
}

void ConfigPanel::applySettings() {
  PLOGI << "Applying settings.";
  Game::settings = settings;

  SetMasterVolume(settings.master_volume);
  SetTargetFPS(settings.framerate);

  if (!on_linux) {
    Game::fullscreenCheck();
  } 

  unapplied = settings != Game::settings;
}

void ConfigPanel::draw() {
  Rectangle source = {0, 0, 232, frame_height};
  DrawTextureRec(frame, source, position, WHITE);

  if (state == PanelState::READY) {
    drawOptions();
  }
}

void ConfigPanel::drawOptions() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  for (int x = 0; x < options.size(); x++) {
    ConfigOption id = options.at(x);

    bool dont_draw = on_linux && id == ConfigOption::FULLSCREEN;
    if (dont_draw) {
      continue;
    } 

    string name = getOptionName(id);
    float y = y_values.at(x);
    Vector2 position = {113, y};

    Color color = WHITE;
    if (unapplied && id == ConfigOption::APPLY) {
      color = Game::palette[26];
    }

    DrawTextEx(*font, name.c_str(), position, txt_size, -2, color);

    if (x <= 4) {
      drawOptionVisuals(id, position, font, txt_size);
    }

    if (id == *selected) {
      drawCursor(position);
    }
  }
}

void ConfigPanel::drawOptionVisuals(ConfigOption id, Vector2 position,
                                    Font *font, int txt_size)
{
  Vector2 dash_position = position;
  dash_position.x = 206;
  DrawTextEx(*font, "-", dash_position, txt_size, -2, WHITE);

  switch (id) {
    case ConfigOption::VOL_MASTER: {
      drawGuage(position, settings.master_volume);
      break;
    }
    case ConfigOption::VOL_SOUND: {
      drawGuage(position, settings.sfx_volume);
      break;
    }
    case ConfigOption::VOL_MUSIC: {
      drawGuage(position, settings.bgm_volume);
      break;
    }
    case ConfigOption::FULLSCREEN: {
      drawCheckbox(position, settings.fullscreen);
      break;
    }
    case ConfigOption::FRAMERATE: {
      drawFramerateStepper(position, font, txt_size);
      break;
    }
    default: {

    }
  }
}

void ConfigPanel::drawGuage(Vector2 position, float percentage) {
  position.x = 231;
  position.y = position.y + 3;

  int segments = percentage * 20;
  for (int x = 0; x < 20; x++) {
    Rectangle *sprite;

    if (x < segments)  {
      sprite = &atlas->sprites[5];
    }
    else {
      sprite = &atlas->sprites[6];
    }

    DrawTextureRec(atlas->sheet, *sprite, position, WHITE);
    position.x += 4;
  }
}

void ConfigPanel::drawCheckbox(Vector2 position, bool enabled) {
  position.x = 264;

  Rectangle *sprite = &atlas->sprites[3 + enabled];
  DrawTextureRec(atlas->sheet, *sprite, position, WHITE);
}

void ConfigPanel::drawFramerateStepper(Vector2 position, Font *font, 
                                       int txt_size) 
{
  Vector2 left_pos = {231, position.y + 2};
  Vector2 right_pos = {302, left_pos.y};

  int framerate = settings.framerate;
  if (framerate != 30) {
    DrawTextureRec(atlas->sheet, atlas->sprites[1], left_pos, WHITE);
  }

  if (framerate != 600) {
    DrawTextureRec(atlas->sheet, atlas->sprites[2], right_pos, WHITE);
  }

  string txt = TextFormat("%i", framerate); 
  Vector2 txt_pos = TextUtils::alignCenter(txt.c_str(), {271, position.y},
                                           *font, -2, 0);
  DrawTextEx(*font, txt.c_str(), txt_pos, txt_size, -3, WHITE);
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
      if (unapplied) {
        return "*APPLY SETTINGS";
      }
      else { 
        return "APPLY SETTINGS";
      }
    }
    case ConfigOption::BACK: {
      return "BACK";
    }
  }
}

