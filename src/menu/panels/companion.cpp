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
#include "utils/text.h"
#include "scenes/setup.h"
#include "menu/hud/portrait.h"
#include "menu/hud/archetype.h"
#include "menu/hud/stat_sheet.h" 
#include "menu/panels/companion.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique;


CompanionPanel::CompanionPanel() {
  id = PanelID::COMPANION;
  atlas = &SetupScene::atlas;
  atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();

  sm_font = &Game::sm_font;
  sm_size = sm_font->baseSize;

  med_font = &Game::med_font;
  med_size = med_font->baseSize;

  keybinds = &Game::settings.menu_keybinds;

  title_sprite = atlas->sprites[1];
  title_sprite.height = 0;

  transition_time = 0.50;

  selected = &options.at(1);
  prev_selected = selected;
  initPortraits();
  startOptionMotion();

  icon = make_unique<Archetype>((Vector2){130, 48});
  icon->source_height = 0;

  stats = make_unique<StatSheet>((Vector2){130, 118}, Game::palette[2]);
  stats->source.height = 0;

  setupCompanionData();

  info_frame1 = LoadTexture("graphics/menu/frames/companion_info.png");
  info_frame2 = LoadTexture("graphics/menu/frames/companion_info2.png");
  PLOGI << "Companion Select Panel has been initialized.";
}

CompanionPanel::~CompanionPanel() {
  UnloadTexture(info_frame1);
  UnloadTexture(info_frame2);

  icon.reset();
  stats.reset();
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

void CompanionPanel::setupCompanionData() {
  text.clear();

  switch (*selected) {
    case CompanionID::ERWIN: {
      text_buffer = "<The Vagabond>";
      name = "Erwin";
      archetype = "MAVERICK";
      main_color = Game::palette[22];

      description = 
        "Erwin's gameplan revolves\n"
        "around disruption and\n"
        "creating opportunities for\n"
        "Mary to capitalize on.\n"
        "Other than that, he's only\n"
        "as good at damage dealing as\n"
        "any other human.";

      stats->life = 20;
      stats->morale = 15;
      stats->offense = 4;
      stats->defense = 4;
      stats->intimid = 6;
      stats->persist = 4;
      stats->dexterity = 5;
      stats->disipline = 5;
      stats->recovery = 100;
      stats->resilience = 50;

      influence.rating = 4;
      independence.rating = 10;
      sustainability.rating = 6;
      break;
    }
    case CompanionID::XANDER: {
      text_buffer = "<The Broken>";
      name = "Xander";
      archetype = "SENTINEL";
      main_color = Game::palette[32];

      description = 
        "Xander is someone who carries\n"
        "a strong devotion towards his\n"
        "dearest friend, Mary.\n"
        "Despite possessing great power,\n"
        "the dragon's large size can make\n"
        "him expensive to maintain, and\n"
        "he is very slow to boot.";

      stats->life = 30;
      stats->morale = 0;
      stats->offense = 12;
      stats->defense = 8;
      stats->intimid = 3;
      stats->persist = 3;
      stats->dexterity = 4;
      stats->disipline = 1;
      stats->recovery = 35;
      stats->resilience = 75;

      influence.rating = 10;
      independence.rating = 6;
      sustainability.rating = 2;
      break;
    }
  }

  text_iter = text_buffer.begin();
  icon->setIcon(*selected);
  arch_position = TextUtils::alignRight(archetype.c_str(), {353, 54}, 
                                        *sm_font, -3, 0);
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
    if (menu_state == INFORMATION && selected != option) {
      continue;
    }

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

  if (menu_state == INFORMATION) {
    float amount = Clamp((-0.60 + percentage) / 0.40, 0.0, 1.0);
    info_source.height = 144 * amount;
    icon->source_height = 70 * amount;
    stats->source.height = 74 * amount;
  }
}

void CompanionPanel::readyLogic() {
  switch (menu_state) {
    case SELECTION: {
      if (opt_clock >= 1.0) {
        optionNavigation();
      }
      else {
        optionMotion();
      }

      for (auto &portrait : portraits) {
        portrait->update(1.0);
      }

      break;
    }
    case INFO_IN: {
      menu_clock += Game::deltaTime() / menu_time;
      menu_clock = Clamp(menu_clock, 0.0, 1.0);

      informationLerp();

      if (menu_clock == 1.0) {
        menu_state = INFORMATION;
      }

      break;
    }
    case INFORMATION: {
      bool gamepad = IsGamepadAvailable(0);
      if (Input::pressed(keybinds->cancel, gamepad)) {
        menu_state = INFO_OUT;
        sfx->play("menu_cancel"); 
      }

      int index = static_cast<int>(*selected);
      unique_ptr<Portrait> &portrait = portraits.at(index);
      portrait->update(1.0);
      break;
    }
    case INFO_OUT: {
      menu_clock -= Game::deltaTime() / menu_time;
      menu_clock = Clamp(menu_clock, 0.0, 1.0);

      informationLerp();

      if (menu_clock == 0.0) {
        menu_state = SELECTION;
      }

      break;
    }
  }

  textScroll();
}

void CompanionPanel::optionNavigation() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->right, gamepad)) {
    prev_selected = selected;
    MenuUtils::nextOption(options, selected, &disallowed, false);

    startOptionMotion();
    setupCompanionData();
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->left, gamepad)) {
    prev_selected = selected;
    MenuUtils::prevOption(options, selected, &disallowed, false);

    startOptionMotion();
    setupCompanionData();
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    int index = static_cast<int>(*selected);
    unique_ptr<Portrait> &portrait = portraits.at(index);
    portrait->fade_clock = 0.0;

    menu_state = INFO_IN;
    sfx->play("menu_select");
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

void CompanionPanel::textScroll() {
  if (scroll_clock < 1.0) {
    scroll_clock += Game::deltaTime() / scroll_time;
  }

  scroll_clock = 0.0;

  if (text_iter != text_buffer.end()) {
    text.push_back(*text_iter);
    text_iter++;

    text_position = TextUtils::alignCenter(text.c_str(), {213, 204}, 
                                           *med_font, 0, 0);
  }
}

void CompanionPanel::informationLerp() {
  for (int x = 0; x < options.size(); x++) {
    CompanionID *option = &options.at(x);
    int index = static_cast<int>(*option);
    unique_ptr<Portrait> &portrait = portraits.at(index);

    if (option == selected) {
      float percentage = Clamp(menu_clock / 0.7, 0.0, 1.0);
      portrait->main_position.x = Math::smoothstep(181, 66, percentage);
      portrait->noise->position.x = portrait->main_position.x + 3;
      portrait->update(1.0);
    }
    else {
      float percentage = Clamp(menu_clock / 0.30, 0.0, 1.0);
      percentage = 1.0 - percentage;

      portrait->update(percentage); 

      portrait->fade_clock = percentage;
    }
  }

  float percentage = Clamp((-0.70 + menu_clock) / 0.30, 0.0, 1.0);
  info_source.height = 144 * percentage;
  icon->source_height = 70 * percentage;
  stats->source.height = 74 * percentage;
}

void CompanionPanel::draw() {
  for (auto &portrait : portraits) {
    portrait->draw();
  }

  if (menu_state != SELECTION) {
    DrawTextureRec(info_frame1, info_source, {200, 48}, main_color);
    icon->draw();
    stats->drawFrame();
  }

  if (state != PanelState::READY) {
    return;
  }

  DrawTextureRec(atlas->sheet, title_sprite, title_position, WHITE);
  DrawTextEx(*med_font, text.c_str(), text_position, med_size, 0, 
             Game::palette[51]);

  if (menu_state == INFORMATION) {
    DrawTexture(info_frame2, 200, 48, WHITE);
    DrawTextEx(*med_font, name.c_str(), {201, 52}, med_size, -2, WHITE);
    DrawTextEx(*sm_font, archetype.c_str(), arch_position, sm_size, -3, 
               main_color);

    stats->drawStats();
    influence.draw();
    independence.draw();
    sustainability.draw();

    SetTextLineSpacing(10);
    DrawTextEx(*sm_font, description.c_str(), {201, 116}, sm_size, -3, 
               WHITE);
    SetTextLineSpacing(16);
  }
}
