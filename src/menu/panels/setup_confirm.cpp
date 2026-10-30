#include <cassert>
#include <cstddef>
#include <memory>
#include <vector>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/panel.h"
#include "utils/math.h"
#include "utils/text.h"
#include "scenes/setup.h"
#include "menu/hud/rating.h"
#include "menu/panels/dialog.h"
#include "menu/panels/setup_confirm.h"
#include <plog/Log.h>

using std::make_unique, std::string, std::vector;


SetupConfirm::SetupConfirm(GameMode mode, SubWeaponID weapon, 
                           CompanionID companion) 
{
  id = PanelID::SETUP_CONFIRM;
  atlas = &SetupScene::atlas;
  atlas->use();

  title_sprite = atlas->sprites[2]; 
  title_sprite.height = 0;

  transition_time = 0.35;

  mary_port.sprite = &mary_port.atlas.sprites[0];

  int sprite_id = static_cast<int>(companion) + 1;
  comp_port.sprite = &comp_port.atlas.sprites[sprite_id];

  med_font = &Game::med_font;
  med_size = med_font->baseSize;
  setupInfo(mode, weapon, companion);
  setupRating(weapon, companion);
  setupDialog();

  info_frame = LoadTexture("graphics/menu/frames/setup_confirm.png");
  PLOGI << "Setup Confirmation Panel has been initialized.";
}

SetupConfirm::~SetupConfirm() {
  UnloadTexture(info_frame);
  stars.reset();
  panel.reset();
  atlas->release();
}

void SetupConfirm::setupInfo(GameMode mode, SubWeaponID weapon, 
                             CompanionID companion)
{
  switch (mode) {
    case GameMode::INDIFFERENCE: {
      m_text = "Indifference";
      break;
    }
    case GameMode::REVERENCE: {
      m_text = "Reverence!";
      m_color = Game::palette[32];
      break;
    }
  }

  m_position = TextUtils::alignRight(m_text.c_str(), {359, 59}, *med_font,
                                     -2, 0);

  switch (weapon) {
    case SubWeaponID::KNIFE: {
      w_text = "Birthright";
      break;
    }
    case SubWeaponID::BAT: {
      w_text = "Heroism";
      break;
    }
  }

  w_position = TextUtils::alignRight(w_text.c_str(), {359, 85}, *med_font,
                                     -2, 0);

  switch (companion) {
    case CompanionID::ERWIN: {
      c_text = "Erwin";
      break;
    }
    case CompanionID::XANDER: {
      c_text = "Xander";
      break;
    }
  }

  c_position = TextUtils::alignRight(c_text.c_str(), {359, 111}, 
                                     *med_font, -2, 0);
}

void SetupConfirm::setupRating(SubWeaponID weapon, CompanionID companion)
{
  assert(stars == nullptr);
  int w_rating;
  switch (weapon) {
    case SubWeaponID::KNIFE: {
      w_rating = 2;
      break;
    }
    case SubWeaponID::BAT: {
      w_rating = 1;
      break;
    }
  }

  int c_rating;
  switch (companion) {
    case CompanionID::ERWIN: {
      c_rating = 3;
      break;
    }
    case CompanionID::XANDER: {
      c_rating = 2;
      break;
    }
  }


  int difficulty = w_rating + c_rating;
  assert(difficulty <= 10);

  float x = std::ceilf(difficulty / 2.0);
  float offset = x * 12;
  Vector2 position = {359 - offset, 140};

  Color color;
  if (difficulty < 5) {
    color = Game::palette[51];
  }
  else if (difficulty < 7) {
    color = Game::palette[22];
  }
  else if (difficulty < 9) {
    color = Game::palette[30];
  }
  else {
    color = Game::palette[32];
  }

  stars = make_unique<StarRating>(position, color);
  stars->rating = difficulty;
}

void SetupConfirm::setupDialog() {
  vector<string> dialog = {
    "\nStart the game?"
  };

  Vector2 position = {195, 147};
  panel = make_unique<DialogPanel>(position, dialog, true, false, true,
                                   true);
}

void SetupConfirm::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    transitionLerp();
    return;
  }

  mary_port.update(1.0);
  comp_port.update(1.0);

  panel->update();
  if (panel->terminate) {
    assert(panel->selected != NULL);
    response = *panel->selected;
    terminate = true;
  }
}

void SetupConfirm::transitionLerp() {
  float percentage;
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  float amount = Clamp(percentage / 0.80, 0.0, 1.0);
  mary_port.update(amount);
  comp_port.update(amount);

  if (state == PanelState::CLOSING) {
    mary_port.fade_clock = amount;
    comp_port.fade_clock = amount;
  }

  start1.y = Math::smoothstep(-1, 44, amount);
  end1.y = start1.y;

  start2.y = Math::smoothstep(241, 195, amount);
  end2.y = start2.y;

  info_source.height = 96 * amount;

  float base_y = atlas->sprites[2].y;
  float height = 24;
  amount = Clamp((-0.70 + percentage) / 0.30, 0.0, 1.0);

  title_sprite.height = height * amount;
  float offset = (height - title_sprite.height) / 2;

  title_sprite.y = base_y + offset;
  title_position.y = 16 + offset;
}

void SetupConfirm::draw() {
  DrawTextureRec(atlas->sheet, title_sprite, title_position, WHITE);
  DrawTextureRec(info_frame, info_source, {197, 48}, WHITE);
  DrawLineV(start1, end1, Game::palette[22]);
  DrawLineV(start2, end2, Game::palette[22]);

  mary_port.draw();
  comp_port.draw();

  if (state == PanelState::READY) {
    DrawTextEx(*med_font, m_text.c_str(), m_position, med_size, -2, 
               m_color);
    DrawTextEx(*med_font, w_text.c_str(), w_position, med_size, -2, 
               WHITE);
    DrawTextEx(*med_font, c_text.c_str(), c_position, med_size, -2, 
               WHITE);

    stars->draw();
    panel->draw();
  }
}
