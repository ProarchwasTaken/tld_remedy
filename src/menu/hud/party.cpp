#include <cassert>
#include <cstddef>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "scenes/camp_menu.h"
#include "menu/hud/party.h"
#include <plog/Log.h>

using std::string;
SpriteAtlas PartyHud::icon_atlas("menu", "status_icons_mini");


PartyHud::PartyHud(Vector2 position) {
  main_position = position;
  atlas = &CampMenuScene::atlas;
  atlas->use();
  icon_atlas.use();
}

PartyHud::~PartyHud() {
  atlas->release();
  icon_atlas.release();
}

void PartyHud::assign(Character *user, float *clock) {
  this->user = user;
  this->clock = clock;
  PLOGI << "Assigned PartyHud to: '" << user->name << "'";
}

void PartyHud::draw() { 
  assert(clock != NULL);

  float percentage = *clock;
  if (reverse) {
    percentage = 1.0 - percentage;
  }
  bool timer_finished = percentage == 1.0;

  Rectangle source = atlas->sprites[7];
  if (!timer_finished) {
    source.height *= percentage;
  }

  DrawTextureRec(atlas->sheet, source, main_position, WHITE);

  if (timer_finished) {
    drawName();
    drawLife();
    drawMorale();
    drawStatusIcons();
  }
}

void PartyHud::drawName() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;
  Vector2 position = Vector2Add(main_position, {2, 1});

  DrawTextEx(*font, user->name, position, txt_size, -2, WHITE);
}

void PartyHud::drawLife() {
  Font *sm_font = &Game::sm_font;
  int sm_size = sm_font->baseSize;

  Font *med_font = &Game::med_font;
  int med_size = med_font->baseSize;

  Color color;
  if (user->life < user->max_life * 0.30) {
    color = Game::palette[33];
  }
  else {
    color = WHITE;
  }

  Vector2 position = Vector2Add(main_position, {2, 16});
  DrawTextEx(*sm_font, "LIFE", position, sm_size, -3, color);

  string life = TextFormat("%02.00f", user->life);
  position.x += 56;
  DrawTextEx(*med_font, life.c_str(), position, med_size, -2, color);

  position.x += 16;
  DrawTextEx(*med_font, "/", position, med_size, 0, WHITE);

  string max_life = TextFormat("%02.00f", user->max_life);
  position.x += 6;
  DrawTextEx(*sm_font, max_life.c_str(), position, sm_size, -3, WHITE);
}

void PartyHud::drawMorale() {
  Font *sm_font = &Game::sm_font;
  int sm_size = sm_font->baseSize;

  Font *med_font = &Game::med_font;
  int med_size = med_font->baseSize;

  Color color;
  if (user->max_morale != 0) {
    color = Game::palette[42];
  }
  else {
    color = Game::palette[2];
  }

  Vector2 position = Vector2Add(main_position, {2, 29});
  DrawTextEx(*sm_font, "MORALE", position, sm_size, -3, color);

  string morale = TextFormat("%02.00f/", user->init_morale);
  position.x += 56;
  DrawTextEx(*med_font, morale.c_str(), position, med_size, -2, color);

  string max_morale = TextFormat("%02.00f", user->max_morale);
  position.x += 22;
  DrawTextEx(*sm_font, max_morale.c_str(), position, sm_size, -3, color);
}

void PartyHud::drawStatusIcons() {
  Vector2 position = Vector2Add(main_position, {64, 4});

  for (int x = 0; x < user->status_limit; x++) {
    StatusID effect = user->status[x];

    if (effect != StatusID::NONE) {
      int id = static_cast<int>(effect);
      Rectangle *sprite = &icon_atlas.sprites.at(id);

      DrawTextureRec(icon_atlas.sheet, *sprite, position, WHITE);
      position.x += 10;
    }
  }
}
