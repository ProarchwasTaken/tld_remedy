#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/party_member.h"
#include "system/sprite_atlas.h"
#include "utils/text.h"
#include "combat/hud/life.h"

SpriteAtlas LifeHud::atlas("hud", "hud_life");


LifeHud::LifeHud(Vector2 position) {
  main_position = position;
  atlas.use();
}

LifeHud::~LifeHud() {
  atlas.release();
}

void LifeHud::assign(PartyMember *combatant) {
  user = combatant;
}

void LifeHud::draw() {
  Font *font = &Game::sm_font;
  int txt_size = font->baseSize;

  decideLifeColor();
  DrawTextureRec(atlas.sheet, atlas.sprites[0], main_position, 
                 life_color);
  drawLifeBar();
  drawLifeText(font, txt_size);
}

void LifeHud::decideLifeColor() {
  if (user->state == CombatantState::DEAD) {
    life_color = Game::palette[32];
  }
  else if (user->critical_life) {
    life_color = Game::palette[33];
  }
  else if (user->exhaustion != 0) {
    life_color = Game::palette[22];
  }
  else {
    life_color = WHITE;
  }
}

void LifeHud::drawLifeBar() {
  float life_percentage = user->life / user->max_life;
  int segments = life_percentage * 10;
  float leftover = (life_percentage * 10) - segments;

  Vector2 position = Vector2Add(main_position, {7, 2});

  for (int x = 0; x < 10; x++) {
    Rectangle *sprite;
    Color color = life_color;

    if (x < segments) {
      sprite = &atlas.sprites[1];
    }
    else if (leftover != 0 && user->state != CombatantState::DEAD) {
      float interval = 1.0 - leftover;
      sprite = segmentBlink(interval);
      leftover = 0;
    }
    else {
      sprite = &atlas.sprites[2];
      color = Game::palette[32];
    }

    DrawTextureRec(atlas.sheet, *sprite, position, color);
    position.x += 7;
  }
}

Rectangle *LifeHud::segmentBlink(float interval) {
  blink_clock += Game::deltaTime() / interval;
  if (blink_clock >= 1.0) {
    segment_blink = !segment_blink;
    blink_clock = 0.0;
  }

  if (segment_blink) {
    return &atlas.sprites[1];
  }
  else {
    return &atlas.sprites[2];
  }
}

void LifeHud::drawLifeText(Font *font, int size) {
  float life = std::floorf(user->life);
  txt_life = TextFormat("%02.00f/%02.00f", life, user->max_life);

  Vector2 txt_position = Vector2Add(main_position, {78, 3});
  txt_position = TextUtils::alignRight(txt_life.c_str(), txt_position, 
                                       *font, -3, 0);

  DrawTextEx(*font, txt_life.c_str(), txt_position, size, -3, life_color);
}

