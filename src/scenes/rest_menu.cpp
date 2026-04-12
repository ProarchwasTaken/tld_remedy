#include <cassert>
#include <cmath>
#include <cstddef>
#include <random>
#include <memory>
#include <string>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "utils/math.h"
#include "utils/menu.h"
#include "utils/input.h"
#include "menu/panels/diagnose.h"
#include "menu/panels/crafting.h"
#include "menu/panels/confirm.h"
#include "menu/panels/dialog.h"
#include "scenes/rest_menu.h"

using std::string, std::vector, std::uniform_int_distribution, 
std::make_unique;


RestMenuScene::RestMenuScene(Session *session) {
  assert(session != NULL);
  scene_id = SceneID::REST_MENU;

  this->session = session;
  keybinds = &Game::settings.menu_keybinds;

  loadBackground(session->location);
  camera.target = {0, 0};
  camera.offset = {213, 120};
  camera.rotation = 0;
  camera.zoom = 0.85;

  black_bars.enabled = false;
  black_bars.setTargetValues(5, 8);
  black_bars.setValues(20, -42);

  atlas.use();
  setupCharacters();

  Game::bgm->prepare("rest");

  sfx = &Game::menu_sfx;
  sfx->use();
}

RestMenuScene::~RestMenuScene() {
  UnloadTexture(background);
  atlas.release();

  sfx->release();
}

void RestMenuScene::loadBackground(string location) {
  background = LoadTexture("graphics/menu/backgrounds/cathedral.png");
  bg_color = Game::palette[40];
  bg_color.a = 0.0;

  char_color = Game::palette[42];
  char_color.a = 0.0;

  black_bars.setTint(Game::palette[43]);
}

void RestMenuScene::setupCharacters() {
  plr_sprite = &atlas.sprites[1];
  plr_position = {59, -21};

  switch (session->companion.companion_id) {
    case CompanionID::ERWIN: {
      com_sprite = &atlas.sprites[2];
      com_position = {4, -21};
    }
  }
}

void RestMenuScene::update() {
  switch (state) {
    case OPENING: {
      openingLogic();
      break;
    }
    case CLOSING: {
      closingLogic();
      break;
    }
    case READY: {
      normalLogic();
      break;
    }
    case OPENING_PANEL: { 
      openingPanel();
      break;
    }
    case CLOSING_PANEL: {
      closingPanel();
      break;
    }
  }

  float now = GetTime();
  float offset_x = std::cosf(now * 0.5) * 8;
  float offset_y = std::sinf(now) * 4;
  camera.target = {offset_x, offset_y};

  black_bars.update(&camera);
}

void RestMenuScene::openingLogic() {
  state_clock += Game::deltaTime() / state_time;
  state_clock = Clamp(state_clock, 0.0, 1.0);

  if (!black_bars.enabled && state_clock >= 0.60) {
    black_bars.enabled = true;
    Game::bgm->play();
    Game::bgm->setBaseVolume(0.0);
    Game::bgm->fade(1.0, 2.0);
  }

  float percentage = Clamp((-0.5 + state_clock) / 0.25, 0.0, 1.0);
  bg_color.a = 255 * percentage;
  char_color.a = 255 * percentage;

  percentage = Clamp((-0.5 + state_clock) / 0.50, 0.0, 1.0);
  camera.zoom = Math::smoothstep(0.85, 1.0, percentage);

  if (state_clock == 1.0) {
    state = READY;
    state_clock = 0.0;
  }
}

void RestMenuScene::closingLogic() {
  state_clock += Game::deltaTime() / state_time;
  state_clock = Clamp(state_clock, 0.0, 1.0);

  float percentage = Clamp(state_clock / 0.40, 0.0, 1.0);
  percentage = 1.0 - percentage;

  bg_color.a = 255 * percentage;
  char_color.a = 255 * percentage;

  percentage = Clamp(state_clock / 0.50, 0.0, 1.0);
  camera.zoom = Math::smoothstep(1.0, 0.85, percentage);


  if (state_clock == 1.0) {
    black_bars.enabled = false;
    Game::bgm->prepare("field");
    Game::bgm->play();
    Game::returnToField();
  }
}

void RestMenuScene::normalLogic() {
  if (opt_clock < 1.0) {
    opt_clock += Game::deltaTime() / opt_time;
    opt_clock = Clamp(opt_clock, 0.0, 1.0);
  }

  flickering();

  if (!panel_mode) {
    optionNavigation();
    return;
  }

  panel->update();

  if (panel->terminate) {
    panelTermination();
  }
}

void RestMenuScene::openingPanel() {
  panel_clock += Game::deltaTime() / panel_time;
  panel_clock = Clamp(panel_clock, 0.0, 1.0);

  bg_color.a = Lerp(255, 127, panel_clock);

  if (panel_clock == 1.0) {
    panel_mode = true;
    state = READY;
  }
}

void RestMenuScene::closingPanel() {
  panel_clock -= Game::deltaTime() / panel_time;
  panel_clock = Clamp(panel_clock, 0.0, 1.0);

  bg_color.a = Lerp(255, 127, panel_clock);

  if (panel_clock == 0.0) {
    state = READY;
  }
}

void RestMenuScene::optionNavigation() {
  if (opt_clock < 1.0) {
    return;
  }

  bool gamepad = IsGamepadAvailable(0);
  if (Input::down(keybinds->down, gamepad)) {
    prev_selected = selected;
    MenuUtils::nextOption(options, selected, &disallowed);

    opt_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::down(keybinds->up, gamepad)) {
    prev_selected = selected;
    MenuUtils::prevOption(options, selected, &disallowed);

    opt_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    selectOption();
    sfx->play("menu_select");
  }
}

void RestMenuScene::selectOption() {
  switch (*selected) {
    case RestMenuOptions::DIAGNOSE: {
      panel = make_unique<DiagnosePanel>(session, &atlas);
      break;
    }
    case RestMenuOptions::CRAFTING: {
      panel = make_unique<CraftingPanel>(session, &atlas);
      break;
    }
    case RestMenuOptions::SAVE: {
      vector<string> dialog = {
        "Record your progress?\n"
        "(Existing data will be overwritten.)"
      };

      Vector2 position = {16, 183};
      panel = make_unique<DialogPanel>(position, dialog, true);
      break;
    }
    case RestMenuOptions::LEAVE: {
      panel = make_unique<ConfirmPanel>(&Game::menu_atlas, keybinds,
                                        "Ready to set off?");
      break;
    }
    default: {
      return;
    }
  }

  prev_selected = NULL;
  state = OPENING_PANEL;
  black_bars.setTargetValues(5, 24);
}

void RestMenuScene::panelTermination() {
  assert(panel != nullptr);
  bool exiting = false;
  switch (panel->id) {
    case PanelID::CONFIRM: {
      ConfirmPanel *confirm = static_cast<ConfirmPanel*>(panel.get());
      if (*confirm->selected == ConfirmOption::YES) {
        exiting = true;
      }
      break;
    }
    case PanelID::DIALOG: {
      DialogPanel *ptr = static_cast<DialogPanel*>(panel.get());

      if (ptr->selected == NULL) {
        break;
      }

      if (*ptr->selected == PromptOptions::YES) {
        Game::saveSession(session);

        panel.reset();

        vector<string> dialog = {"Progress has been saved."};
        Vector2 position = {16, 183};
        panel = make_unique<DialogPanel>(position, dialog);
        return;
      }
    }
    default: {
      break;
    }
  }

  panel.reset();
  panel_mode = false;

  if (!exiting) {
    black_bars.setTargetValues(5, 8);
    state = CLOSING_PANEL;
  }
  else {
    black_bars.setTargetValues(20, -56);
    Game::bgm->fade(0.0, 1.0);
    state = CLOSING;
  }
}

void RestMenuScene::flickering() {
  tick_clock += Game::deltaTime() / tick_time;
  if (tick_clock < 1.0) {
    return;
  }

  tick_clock = 1.0;

  bool end = false;
  if (char_color.a != 255) {
    char_color.a = 255;
    end = true;
  }

  int bg_limit = 255;
  int bg_min = 200;
  if (panel_mode) {
    bg_limit = 127;
    bg_min = 60;
  }

  if (bg_color.a != bg_limit) {
    bg_color.a = bg_limit;
    end = true;
  }

  if (end) {
    return;
  }

  uniform_int_distribution<int> range(1, 50);
  int number = range(Game::RNG);
  if (number == 50) {
    char_color.a = 200;
  }

  number = range(Game::RNG);
  if (number == 50) {
    bg_color.a = bg_min;
  }
}

void RestMenuScene::draw() {
  drawBackground();
  drawOptions();
  
  if (panel_mode) {
    panel->draw();
  }
}

void RestMenuScene::drawBackground() {
  BeginMode2D(camera);
  { 
    DrawTextureV(background, {-250, -150}, bg_color);
    black_bars.draw();
    DrawTextureRec(atlas.sheet, *plr_sprite, plr_position, char_color);
    DrawTextureRec(atlas.sheet, *com_sprite, com_position, char_color);
  }
  EndMode2D();
}

void RestMenuScene::drawOptions() {
  if (state == CLOSING) {
    return;
  }

  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  int offset = (options.size() - 1) - disallowed.size();
  Vector2 base_position = {-16.0f, 226.0f - (offset * 16)};
  Color base_color = Game::palette[2];
  baseOptionLerp(base_position, base_color);

  for (RestMenuOptions &option : options) {
    if (disallowed.find(option) != disallowed.end()) {
      continue;
    }

    Vector2 position = base_position;
    Color color = base_color;

    if (&option == selected) {
      selectedLogic(opt_clock, position, color);
    }
    else if (&option == prev_selected) {
      selectedLogic(1.0 - opt_clock, position, color);
    }
    else {
      unselectedLogic(position, color);
    }

    DrawTextureRec(atlas.sheet, atlas.sprites[0], position, color);

    const char *text = getOptionText(option);
    position = Vector2Add(position, {23, -10});
    DrawTextEx(*font, text, position, txt_size, -2, color);

    base_position.y += 16;
  }
}

void RestMenuScene::baseOptionLerp(Vector2 &position, Color &color) {
  if (state == OPENING) {
    float percentage = Clamp((-0.70 + state_clock) / 0.30, 0.0, 1.0);
    position.x = Math::smoothstep(position.x - 32, position.x, 
                                  percentage);
    color.a = 255 * percentage; 
  }
}

void RestMenuScene::selectedLogic(float clock, Vector2 &position, 
                                  Color &color)
{
  if (state == OPENING_PANEL || state == CLOSING_PANEL) {
    float percentage = Clamp((-0.75 + panel_clock) / 0.25, 0.0, 1.0);
    percentage = 1.0 - percentage;

    color = WHITE;
    color.a = 255 * percentage;
    position.x += 16;
    return;
  }
  else if (state == READY && panel_mode) {
    color = WHITE;
    color.a = 0;
    return;
  }

  if (clock == 1.0) {
    color = WHITE;
    position.x += 16;
    return;
  }
  else if (clock == 0.0) {
    return;
  }

  position.x += Math::smoothstep(0, 16, clock);

  Color start_color = Game::palette[2];
  unsigned char r = Lerp(start_color.r, 255, clock);
  unsigned char g = Lerp(start_color.g, 255, clock);
  unsigned char b = Lerp(start_color.b, 255, clock);

  color = {r, g, b, 255};
}

void RestMenuScene::unselectedLogic(Vector2 &position, Color &color) {
  if (state == OPENING_PANEL || state == CLOSING_PANEL) {
    float percentage = Clamp(panel_clock / 0.5, 0.0, 1.0);
    position.x = Math::smoothstep(position.x, position.x - 32, 
                                  percentage);

    percentage = 1.0 - percentage;
    color.a = 255 * percentage;
  }
  else if (state == READY && panel_mode) {
    color.a = 0;
  }
}

const char *RestMenuScene::getOptionText(RestMenuOptions option) {
  switch (option) {
    case RestMenuOptions::DIAGNOSE: {
      return "DIAGNOSE";
    }
    case RestMenuOptions::CRAFTING: {
      return "CRAFTING";
    }
    case RestMenuOptions::TALK: {
      return "TALK";
    }
    case RestMenuOptions::SAVE: {
      return "SAVE";
    }
    case RestMenuOptions::LEAVE: {
      return "LEAVE";
    }
  }
}
