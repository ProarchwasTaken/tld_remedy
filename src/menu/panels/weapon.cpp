#include <array>
#include <cassert>
#include <iterator>
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/keybinds.h"
#include "utils/menu.h"
#include "utils/input.h"
#include "utils/math.h"
#include "scenes/setup.h"
#include "menu/hud/w_portrait.h"
#include "menu/panels/weapon.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique;


WeaponPanel::WeaponPanel() {
  id = PanelID::WEAPON;
  atlas = &SetupScene::atlas;
  atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();

  keybinds = &Game::settings.menu_keybinds;

  selected = &options.at(2);
  prev_selected = selected;
  initPortraits();
  startOptionMotion();
  
  gradient = LoadTexture("graphics/overlays/weapon_gradient.png");
  PLOGI << "Weapon Select Panel has been initialized.";
}

WeaponPanel::~WeaponPanel() {
  UnloadTexture(gradient);
  for (auto &portrait : portraits) {
    portrait.reset();
  }

  atlas->release();
  sfx->release();
}

void WeaponPanel::initPortraits() {
  PLOGI << "Initializing weapon portraits.";

  for (int x = 0; x < 5; x++) {
    WeaponOption *option = &options.at(x);
    int index = static_cast<int>(*option);
    int distance = std::distance(selected, option);
    PLOGD << "Distance from selected: " << distance;

    int sprite_id = index + 1;
    Vector2 position = option_position;
    float offset = opt_spacing * distance;
    position.x += offset;

    unique_ptr<WeaponPortrait> &portrait = portraits.at(index);
    portrait = make_unique<WeaponPortrait>(position, sprite_id);
  }
}

void WeaponPanel::update() {
  if (opt_clock >= 1.0) {
    optionNavigation();
  }
  else {
    optionMotion();
  }

  for (auto &portrait : portraits) {
    portrait->update();
  }
}

void WeaponPanel::optionNavigation() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->right, gamepad)) {
    prev_selected = selected;
    MenuUtils::nextOption(options, selected, &disallowed, false);
    startOptionMotion();
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->left, gamepad)) {
    prev_selected = selected;
    MenuUtils::prevOption(options, selected, &disallowed, false);
    startOptionMotion();
    sfx->play("menu_navigate");
  }
}

void WeaponPanel::startOptionMotion() {
  int distance = std::distance(prev_selected, selected);
  float offset = opt_spacing * distance;

  opt_start_x = option_position.x;
  opt_end_x = opt_start_x + offset;
  opt_clock = 0.0;
}

void WeaponPanel::optionMotion() {
  opt_clock += Game::deltaTime() / opt_time;
  opt_clock = Clamp(opt_clock, 0.0, 1.0);

  option_position.x = Math::smoothstep(opt_end_x, opt_start_x, opt_clock);

  for (int x = 0; x < 5; x++) {
    WeaponOption *option = &options.at(x);
    int index = static_cast<int>(*option);
    int distance = std::distance(selected, option);
    float offset = opt_spacing * distance;

    unique_ptr<WeaponPortrait> &portrait = portraits.at(index);

    assert(portrait != nullptr);
    portrait->main_position.x = option_position.x + offset;

    if (option == selected) {
      selectedLogic(portrait.get());
    }
    else if (option == prev_selected) {
      prevSelectedLogic(portrait.get());
    }
  }
}

void WeaponPanel::selectedLogic(WeaponPortrait *portrait) {
  Color start_color = Game::palette[40];
  Color end_color = Game::palette[42];

  unsigned char r = Lerp(start_color.r, end_color.r, opt_clock);
  unsigned char g = Lerp(start_color.g, end_color.g, opt_clock);
  unsigned char b = Lerp(start_color.b, end_color.b, opt_clock);

  portrait->color = {r, g, b, 255};
  portrait->active = true;
}

void WeaponPanel::prevSelectedLogic(WeaponPortrait *portrait) {
  Color start_color = Game::palette[42];
  Color end_color = Game::palette[40];

  unsigned char r = Lerp(start_color.r, end_color.r, opt_clock);
  unsigned char g = Lerp(start_color.g, end_color.g, opt_clock);
  unsigned char b = Lerp(start_color.b, end_color.b, opt_clock);

  portrait->color = {r, g, b, 255};
  portrait->active = false; 
}

void WeaponPanel::draw() {
  for (auto &portrait : portraits) {
    portrait->draw();
  }

  DrawTexture(gradient, 0, 0, WHITE);

  DrawTextureRec(atlas->sheet, atlas->sprites[0], {16, 16}, WHITE);
}
