#include <array>
#include <cassert>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/panel.h"
#include "data/keybinds.h"
#include "utils/menu.h"
#include "utils/input.h"
#include "utils/math.h"
#include "utils/text.h"
#include "scenes/setup.h"
#include "menu/hud/w_portrait.h"
#include "menu/hud/stat_sheet.h"
#include "menu/panels/weapon.h"
#include <plog/Log.h>

using std::unique_ptr, std::make_unique;


WeaponPanel::WeaponPanel() {
  id = PanelID::WEAPON;
  atlas = &SetupScene::atlas;
  atlas->use();

  sfx = &Game::menu_sfx;
  sfx->use();

  sm_font = &Game::sm_font;
  sm_size = sm_font->baseSize;

  med_font = &Game::med_font;
  med_size = med_font->baseSize;

  keybinds = &Game::settings.menu_keybinds;

  title_sprite = atlas->sprites[0];
  title_sprite.height = 0;

  transition_time = 1.5;

  selected = &options.at(2);
  prev_selected = selected;
  initPortraits();
  startOptionMotion();

  stats = make_unique<StatSheet>((Vector2){153, 102}, Game::palette[40]);
  stats->source.height = 0;
  setupWeaponData();
  
  gradient = LoadTexture("graphics/overlays/weapon_gradient.png");
  info_frame = LoadTexture("graphics/menu/frames/weapon_info.png");
  PLOGI << "Weapon Select Panel has been initialized.";
}

WeaponPanel::~WeaponPanel() {
  UnloadTexture(gradient);
  UnloadTexture(info_frame);

  stats.reset();
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
    position.y = -64;

    unique_ptr<WeaponPortrait> &portrait = portraits.at(index);
    portrait = make_unique<WeaponPortrait>(position, sprite_id);

    if (disallowed.find(*option) != disallowed.end()) {
      portrait->color.a = 0;
    }
  }
}

void WeaponPanel::setupWeaponData() {
  name.clear();
  type.clear();
  blurb.clear();
  stats->resetStats();

  switch (*selected) {
    case WeaponOption::BIRTHRIGHT: {
      name_buffer = "<BIRTHRIGHT>";
      type_buffer = "knifekind Sub-Weapon";
      blurb_buffer = "\"The choice had always been in your hands.\"";
      
      tech1_name = "<Cleave>";
      tech1_cost = "6 MP";

      tech2_name = "<Piercer>";
      tech2_cost = "8 MP";

      description = 
        "A simple kitchen knife\n"
        "that can be found in\n"
        "your local silverware\n"
        "drawer.\n"
        "Still a reliable weapon\n"
        "nonetheless.";

      stats->intimid = 2;
      stats->dexterity = 2;
      break;
    }
    case WeaponOption::HEROISM: {
      name_buffer = "<HEROISM>";
      type_buffer = "batkind Sub-Weapon";
      blurb_buffer = "\"The responsibility and the weight that comes "
        "with it.\"";

      tech1_name = "<Swing>";
      tech1_cost = "5 MP";

      tech2_name = "<Hail Mary>";
      tech2_cost = "3 MP";

      description = 
        "Possesses great\n"
        "damage potential, but\n"
        "unwieldly in untrained\n"
        "hands. Misuse of the\n"
        "weapon may leave you\n"
        "vulnerable for a time.";

      stats->offense = 5;
      stats->intimid = 2;
      stats->morale = -5;
      stats->disipline = -2;
      stats->recovery = -15;
      break;
    }
    case WeaponOption::BRAVERY: {
      name_buffer = "<BRAVERY>";
      type_buffer = "shieldkind Sub-Weapon";
      blurb_buffer = "\"The innate instinct to keep moving forward.\"";

      tech1_name = "<Parry>";
      tech1_cost = "4 MP";

      tech2_name = "<Charge>";
      tech2_cost = "6 MP";

      description = 
        "A makeshift shield\n"
        "that is held together\n"
        "hopes and dreams.\n"
        "Great for the playstyle\n"
        "of protecting your\n"
        "friends!";

      stats->life = 5;
      stats->defense = 2;
      stats->persist = 2;
      stats->resilience = 15;
      stats->recovery = -25;
      break;
    }
    case WeaponOption::DEFIANCE: {
      name_buffer = "<DEFIANCE>";
      type_buffer = "lightkind Sub-Weapon";
      blurb_buffer = "\"Even the faintest of lights can dispel the "
        "blackest of darkness.\"";

      tech1_name = "<Pulse>";
      tech1_cost = "6 MP";

      tech2_name = "<Focus>";
      tech2_cost = "8 MP/s";

      description = 
        "An UV flashlight that\n"
        "is capable of blinding\n"
        "enemies.\n"
        "Has great utility when\n"
        "used in conjunction\n"
        "with Companions.";

      stats->morale = 5;
      stats->intimid = 4;
      stats->disipline = 2;
      stats->persist = -2;
      stats->resilience = -10;
      break;
    }
    case WeaponOption::SPECIALIST: {
      name_buffer = "<SPECIALIST>";
      type_buffer = "needlekind Sub-Weapon";
      blurb_buffer = "\"Staying true to yourself may give you the edge "
        "you need.\"";

      description =
        "A familiar set of\n"
        "surgical needles.\n"
        "Has great support and\n"
        "zoning capabilities,\n"
        "but it's damage output\n"
        "is weak on it's own.";

      tech1_name = "<Pinpoint>";
      tech1_cost = "4 MP";

      tech2_name = "<Remedy>";
      tech2_cost = "ALL MP";
 
      stats->dexterity = 4;
      stats->disipline = 2;
      stats->recovery = 15;
      stats->offense = -2;
      stats->resilience = -20;
      break;
    }
  }

  name_iter = name_buffer.begin();
  type_iter = type_buffer.begin();
  blurb_iter = blurb_buffer.begin();
}

void WeaponPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    transitionLerp();
  }
  else {
    readyLogic();
  } 

  for (auto &portrait : portraits) {
    portrait->update();
  }
}

void WeaponPanel::transitionLerp() { 
  float percentage;
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  for (int x = 0; x < 5; x++) {
    WeaponOption *option = &options.at(x);
    int index = static_cast<int>(*option);
    int distance = std::distance(selected, option);
    PLOGD << "Distance from selected: " << distance;

    float delay = -0.15 * std::abs(distance);
    float amount = Clamp((delay + percentage) / 0.60, 0, 1.0);

    unique_ptr<WeaponPortrait> &portrait = portraits.at(index);
    portrait->main_position.y = Math::smoothstep(-64, 120, amount);
  }

  float height = 24;
  float amount = Clamp((-0.70 + percentage) / 0.30, 0.0, 1.0);

  title_sprite.height = height * amount;
  float offset = (height - title_sprite.height) / 2;

  title_sprite.y = (height - title_sprite.height) / 2;
  title_position.y = 16 + offset;

  if (menu_state == INFORMATION) {
    float amount = Clamp((-0.60 + percentage) / 0.40, 0.0, 1.0);
    info_source.height = 112 * amount;
    stats->source.height = 74 * amount;
  }
}

void WeaponPanel::readyLogic() {
  switch (menu_state) {
    case SELECTION: {
      if (opt_clock >= 1.0) {
        optionNavigation();
      }
      else {
        optionMotion();
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
      confirmInput(); 
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

void WeaponPanel::optionNavigation() {
  bool gamepad = IsGamepadAvailable(0);

  if (Input::pressed(keybinds->right, gamepad)) {
    prev_selected = selected;
    MenuUtils::nextOption(options, selected, &disallowed, false);

    startOptionMotion();
    setupWeaponData();
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->left, gamepad)) {
    prev_selected = selected;
    MenuUtils::prevOption(options, selected, &disallowed, false);

    startOptionMotion();
    setupWeaponData();
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybinds->confirm, gamepad)) {
    menu_state = INFO_IN;
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    state = PanelState::CLOSING;
    exiting_out = true;
    sfx->play("menu_cancel");
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

void WeaponPanel::textScroll() {
  if (scroll_clock < 1.0) {
    scroll_clock += Game::deltaTime() / scroll_time;
  }

  scroll_clock = 0.0;

  if (name_iter != name_buffer.end()) {
    name.push_back(*name_iter);
    name_iter++;

    name_position = TextUtils::alignCenter(name.c_str(), {213, 192}, 
                                           *med_font, 0, 0);
  } 

  if (type_iter != type_buffer.end()) {
    type.push_back(*type_iter);
    type_iter++;

    type_position = TextUtils::alignCenter(type.c_str(), {213, 206}, 
                                           *sm_font, -3, 0);
  }

  if (blurb_iter != blurb_buffer.end()) {
    blurb.push_back(*blurb_iter);
    blurb_iter++;

    blurb_position = TextUtils::alignCenter(blurb.c_str(), {213, 217}, 
                                            *sm_font, -3, 0);
  }
}

void WeaponPanel::informationLerp() {
  for (int x = 0; x < 5; x++) {
    WeaponOption *option = &options.at(x);
    int index = static_cast<int>(*option);
    unique_ptr<WeaponPortrait> &portrait = portraits.at(index);

    if (option == selected) {
      float percentage = Clamp(menu_clock / 0.7, 0.0, 1.0);
      portrait->main_position.x = Math::smoothstep(213, 121, percentage);
    }
    else if (disallowed.find(*option) == disallowed.end()) {
      float percentage = Clamp(menu_clock / 0.30, 0.0, 1.0);
      portrait->main_position.y = Math::smoothstep(120, 140, percentage);
      portrait->color.a = Math::smoothstep(255, 0, percentage);
    }
  }

  float percentage = Clamp((-0.70 + menu_clock) / 0.30, 0.0, 1.0);
  info_source.height = 112 * percentage;
  stats->source.height = 74 * percentage;
}

void WeaponPanel::confirmInput() {
  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->confirm, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybinds->cancel, gamepad)) {
    menu_state = INFO_OUT;
    sfx->play("menu_cancel");
  }
}

void WeaponPanel::draw() {
  for (auto &portrait : portraits) {
    portrait->draw();
  }

  if (menu_state != SELECTION) {
    DrawTextureRec(info_frame, info_source, {153, 64}, WHITE);
    stats->drawFrame();
  }

  DrawTexture(gradient, 0, 0, WHITE);
  DrawTextureRec(atlas->sheet, title_sprite, title_position, WHITE);

  if (state != PanelState::READY) {
    return;
  }

  DrawTextEx(*med_font, name.c_str(), name_position, med_size, 0, 
             Game::palette[42]);
  DrawTextEx(*sm_font, type.c_str(), type_position, sm_size, -3, 
             Game::palette[22]);
  DrawTextEx(*sm_font, blurb.c_str(), blurb_position, sm_size, -3, 
             Game::palette[2]);

  if (menu_state == INFORMATION) {
    stats->drawStatChanges();
    drawTech({154, 71}, tech1_name.c_str(), tech1_cost.c_str());
    drawTech({154, 86}, tech2_name.c_str(), tech2_cost.c_str());

    SetTextLineSpacing(10);
    DrawTextEx(*sm_font, description.c_str(), {224, 108}, sm_size, -3,
               WHITE);
    SetTextLineSpacing(16);
  }
}

void WeaponPanel::drawTech(Vector2 position, const char *name, 
                           const char *cost)
{
  DrawTextEx(*med_font, name, position, med_size, -2, Game::palette[42]);

  position.x += 177;
  position = TextUtils::alignRight(cost, position, *med_font, -2, 0);
  DrawTextEx(*med_font, cost, position, med_size, -2, Game::palette[42]);
}
