#include <cassert>
#include <memory>
#include <cmath>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/party_member.h"
#include "base/status_effect.h"
#include "system/sprite_atlas.h"
#include "utils/text.h"
#include "combat/hud/life.h"
#include <plog/Log.h>

using std::string, std::unique_ptr;
SpriteAtlas LifeHud::atlas("hud", "hud_life");
SpriteAtlas LifeHud::bust_atlas("hud", "");
SpriteAtlas LifeHud::status_atlas("hud", "status_icons");


LifeHud::LifeHud(Vector2 position) {
  main_position = position;
  morale_color = Game::palette[42];
  atlas.use();
  status_atlas.use();
}

LifeHud::~LifeHud() {
  atlas.release();
  bust_atlas.release();
  status_atlas.release();
}

void LifeHud::assign(PartyMember *combatant) {
  user = combatant;

  string sprite_group;
  switch (combatant->id) {
    case PartyMemberID::MARY: {
      sprite_group = "mary_bust";
      break;
    }
    default: {
      PLOGE << "There are no bust graphics for this combatant!";
    }
  }

  bust_atlas = SpriteAtlas("hud", sprite_group);
  bust_atlas.use();
  PLOGD << "Lifehud instance assigned to Combatant: '" 
    << user->name << "'";
}

void LifeHud::update() {
  decideLifeColor();
}

void LifeHud::decideLifeColor() {
  if (user->state == CombatantState::DEAD) {
    life_color = Game::palette[32];
  }
  else if (user->critical_life) {
    life_color = criticalFlash();
  }
  else {
    life_color = WHITE;
  }
}

Color LifeHud::criticalFlash() {
  flash_clock += Game::deltaTime() / 0.35;
  if (flash_clock >= 1.0) {
    flashed = !flashed;
  }

  if (flashed) {
    return Game::palette[34];
  }
  else {
    return Game::palette[33];
  }
}

void LifeHud::draw() {
  Font *font = &Game::sm_font;
  int txt_size = font->baseSize;

  drawBustGraphic();
  drawStatusIcons();
  drawLife(font, txt_size);

  if (user->max_morale != 0) {
    drawMorale(font, txt_size);
  }
}

void LifeHud::drawBustGraphic() {
  assert(bust_atlas.users() != 0);

  Vector2 position = Vector2Subtract(main_position, {26, 49});
  Color tint = user->spriteTint();
  tint.a = 255;
  DrawTextureRec(bust_atlas.sheet, bust_atlas.sprites[0], position, tint);
}

void LifeHud::drawStatusIcons() {
  int count = user->status.size();
  if (count == 0) {
    return;
  }

  Vector2 base_position = Vector2Subtract(main_position, {18, 5});

  for (int x = 0; x < count; x++) {
    StatusEffect *effect = user->status.at(x).get();
    StatusID id = effect->id;
    Rectangle *sprite = getIconSprite(id);

    bool odd = x % 2 != 0;

    Vector2 offset = {10.0f * odd, 10.0f * x};
    Vector2 position = Vector2Subtract(base_position, offset);
    DrawTextureRec(status_atlas.sheet, *sprite, position, WHITE);
  }
}

Rectangle *LifeHud::getIconSprite(StatusID id) {
  int index = static_cast<int>(id);
  return &status_atlas.sprites.at(index);
}

void LifeHud::drawLife(Font *font, int txt_size) {
  DrawTextureRec(atlas.sheet, atlas.sprites[0], main_position, 
                 life_color);
  drawLifeSegments();
  drawLifeText(font, txt_size);
}

void LifeHud::drawLifeSegments() {
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
      float interval = (1.0 - leftover) * 0.5;
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

  Vector2 position = Vector2Add(main_position, {78, 3});
  position = TextUtils::alignRight(txt_life.c_str(), position, *font, -3, 
                                   0);

  DrawTextEx(*font, txt_life.c_str(), position, size, -3, life_color);
}

void LifeHud::drawMorale(Font *font, int txt_size) {
  Vector2 position = Vector2Add(main_position, {14, -10});
  DrawTextureRec(atlas.sheet, atlas.sprites[3], position, morale_color);

  drawMoraleGauge();
  drawMoraleText(font, txt_size);
}

void LifeHud::drawMoraleGauge() {
  Vector2 position = Vector2Add(main_position, {21, -2});
  float morale_percentage = Clamp(user->morale / user->max_morale, 
                                  0.0, 1.0);

  Rectangle gauge = atlas.sprites[4];
  gauge.width = gauge.width * morale_percentage;

  DrawTextureRec(atlas.sheet, atlas.sprites[5], position, WHITE);
  DrawTextureRec(atlas.sheet, gauge, position, morale_color);
}

void LifeHud::drawMoraleText(Font *font, int size) {
  txt_morale = TextFormat("%02.02f", user->morale);

  Vector2 position = Vector2Add(main_position, {92, -12});
  position = TextUtils::alignRight(txt_morale.c_str(), position, *font, 
                                   -3, 0);

  DrawTextEx(*font, txt_morale.c_str(), position, size, -3, morale_color);
}
