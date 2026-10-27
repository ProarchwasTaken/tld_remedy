#include <cassert>
#include <iterator>
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/panel.h"
#include "utils/input.h"
#include "utils/menu.h"
#include "utils/math.h"
#include "scenes/setup.h"
#include "menu/hud/portrait.h"
#include "menu/panels/companion.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique;


CompanionPanel::CompanionPanel() {
  id = PanelID::COMPANION;
  atlas = &SetupScene::atlas;
  atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();

  keybinds = &Game::settings.menu_keybinds;

  title_sprite = atlas->sprites[1];
  title_sprite.height = 0;

  transition_time = 0.50;

  selected = &options.at(1);
  prev_selected = selected;
  initPortraits();
  startOptionMotion();
  PLOGI << "Companion Select Panel has been initialized.";
}

CompanionPanel::~CompanionPanel() {
  for (auto &portrait : portraits) {
    portrait.reset();
  }

  atlas->release();
  sfx->release();
}

void CompanionPanel::initPortraits() {
  PLOGI << "Initializing companion portraits.";

  for (int x = 0; x < options.size(); x++)  {
    CompanionID *option = &options.at(x);
    int index = static_cast<int>(*option);
    int distance = std::distance(selected, option);
    PLOGD << "Distance from selected: " << distance;

    int sprite_id = index + 1;
    Vector2 position = option_position;
    float offset = opt_spacing * distance;
    position.x += offset;

    unique_ptr<Portrait> &portrait = portraits.at(index);
    portrait = make_unique<Portrait>(position);
    portrait->sprite = &portrait->atlas.sprites[sprite_id];
    portrait->noise->setAlpha(0.40);
  }

  PLOGI << "Portraits initialized.";
}

void CompanionPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    transitionLerp();
  }
  else {
    readyLogic();
  }
}

void CompanionPanel::transitionLerp() {
  float percentage;
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  for (int x = 0; x < options.size(); x++) {
    CompanionID *option = &options.at(x);
    int index = static_cast<int>(*option);
    int distance = std::distance(selected, option);

    float delay = -0.15 * std::abs(distance);
    float amount = Clamp((delay + percentage) / 0.50, 0, 1.0);

    unique_ptr<Portrait> &portrait = portraits.at(index);
    portrait->update(amount);

    if (state == PanelState::CLOSING) {
      amount = Clamp((delay + percentage) / 0.85, 0.0, 1.0);
      portrait->fade_clock = amount;
    }
  }

  float base_y = atlas->sprites[1].y;
  float height = 24;
  float amount = Clamp((-0.70 + percentage) / 0.30, 0.0, 1.0);

  title_sprite.height = height * amount;
  float offset = (height - title_sprite.height) / 2;

  title_sprite.y = base_y + offset;
  title_position.y = 16 + offset;
}

void CompanionPanel::readyLogic() {
  if (opt_clock >= 1.0) {
    optionNavigation();
  }
  else {
    optionMotion();
  }

  for (auto &portrait : portraits) {
    portrait->update(1.0);
  }
}

void CompanionPanel::optionNavigation() {
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
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    exiting_out = true;
    sfx->play("menu_cancel");
  }
}

void CompanionPanel::startOptionMotion() {
  int distance = std::distance(prev_selected, selected);
  float offset = opt_spacing * distance;

  opt_start_x = option_position.x;
  opt_end_x = opt_start_x + offset;
  opt_clock = 0.0;
}

void CompanionPanel::optionMotion() {
  opt_clock += Game::deltaTime() / opt_time;
  opt_clock = Clamp(opt_clock, 0.0, 1.0);

  option_position.x = Math::smoothstep(opt_end_x, opt_start_x, opt_clock);

  for (int x = 0; x < options.size(); x++) {
    CompanionID *option = &options.at(x);
    int index = static_cast<int>(*option);
    int distance = std::distance(selected, option);
    float offset = opt_spacing * distance;

    unique_ptr<Portrait> &portrait = portraits.at(index);

    assert(portrait != nullptr);
    portrait->main_position.x = option_position.x + offset;
    portrait->noise->position.x = option_position.x + offset + 3;

    if (option == selected) {
      selectedLogic(portrait.get());
    }
    else if (option == prev_selected) {
      prevSelectedLogic(portrait.get());
    }
  }
}

void CompanionPanel::selectedLogic(Portrait *portrait) {
  float alpha = Math::smoothstep(0.40, 0.05, opt_clock);
  portrait->noise->setAlpha(alpha);
}

void CompanionPanel::prevSelectedLogic(Portrait *portrait) {
  float alpha = Math::smoothstep(0.05, 0.40, opt_clock);
  portrait->noise->setAlpha(alpha);
}


void CompanionPanel::draw() {
  for (auto &portrait : portraits) {
    portrait->draw();
  }

  DrawTextureRec(atlas->sheet, title_sprite, title_position, WHITE);
}
