#include <cassert>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "base/party_member.h"
#include "system/sprite_atlas.h"
#include "utils/text.h"
#include "combat/combatants/party/mary.h"
#include "combat/hud/enemy.h"
#include <plog/Log.h>

using std::string;
SpriteAtlas EnemyHud::atlas("hud", "hud_enemy");


EnemyHud::EnemyHud(Vector2 position) {
  main_position = position;

  atlas.use();
  life_bar = atlas.getTexturefromSprite(0);
  life_empty = atlas.getTexturefromSprite(1);
}

EnemyHud::~EnemyHud() {
  UnloadTexture(life_bar);
  UnloadTexture(life_empty);
  atlas.release();
}

void EnemyHud::assign(Mary *&player, PartyMember *&companion) {
  this->player = &player;
  this->companion = &companion;
}

void EnemyHud::update() {

}

void EnemyHud::draw() {
  if (*player == NULL) {
    return;
  }

  Mary *mary = *player;
  if (mary->target != NULL) {
    drawTargetHud(mary->target, main_position);
  }
}

void EnemyHud::drawTargetHud(Combatant *target, Vector2 position) {
  float width_percentage = target->max_life / LIFE_PER_SEGMENT;

  float max_gauge_width = 136;
  float gauge_width = max_gauge_width * width_percentage;

  float frame_width;
  bool overflow = gauge_width > max_gauge_width;
  if (!overflow) {
    frame_width = gauge_width + 4;
  }
  else {
    frame_width = max_gauge_width + 4;
  }

  Rectangle frame = {position.x, position.y, frame_width, 7};
  DrawRectanglePro(frame, {frame_width, 0}, 0, BLACK);

  drawName(target, position);
  drawGauge(target, position, gauge_width, max_gauge_width, overflow);

  if (overflow) {
    drawSegments(target, position);
  }
}

void EnemyHud::drawGauge(Combatant *target, Vector2 position, float width,
                         float max_width, bool overflow)
{
  Rectangle source = {0, 0, -width, 3};
  Rectangle dest = source;
  dest.x = position.x - 2;
  dest.y = position.y + 2;

  if (!overflow) {
    dest.width = width;
    DrawTexturePro(life_empty, source, dest, {width, 0}, 0, WHITE);

    float percentage = target->life / target->max_life;

    width = width * percentage;
    source.width = -width;
    dest.width = width;
    DrawTexturePro(life_bar, source, dest, {width, 0}, 0, WHITE);
    return;
  }

  int segments = std::floorf(target->life / LIFE_PER_SEGMENT);

  Texture *empty;
  Color color;
  if (segments == 0) {
    empty = &life_empty;
    color = WHITE;
  }
  else {
    empty = &life_bar;
    color = Game::palette[2];
  }

  dest.width = max_width;
  source.width = -max_width;
  DrawTexturePro(*empty, source, dest, {max_width, 0}, 0, color); 

  float leftover = target->life - (LIFE_PER_SEGMENT * segments);
  float percentage = leftover / LIFE_PER_SEGMENT;

  width = max_width * percentage;
  source.width = -width;
  dest.width = width;
  DrawTexturePro(life_bar, source, dest, {width, 0}, 0, WHITE);
}

void EnemyHud::drawSegments(Combatant *target, Vector2 position) {
  assert(target->max_life / LIFE_PER_SEGMENT > 1.0);
  int max_segments = std::floorf(target->max_life / LIFE_PER_SEGMENT);
  int segments = std::floorf(target->life / LIFE_PER_SEGMENT);

  position.y += 5;
  float frame_width = 2 + (6 * max_segments);
  Rectangle frame = {position.x, position.y, frame_width, 4};
  DrawRectanglePro(frame, {frame_width, 0}, 0, BLACK);

  position = Vector2Add(position, {-1, 1});
  for (int x = 0; x < max_segments; x++) {
    position.x -= 6;

    Rectangle *sprite;
    if (x < segments) {
      sprite = &atlas.sprites[2];
    }
    else {
      sprite = &atlas.sprites[3];
    }

    DrawTextureRec(atlas.sheet, *sprite, position, WHITE);
  }
}

void EnemyHud::drawName(Combatant *target, Vector2 position) {
  string name = target->name;

  for (char &letter : name) {
    letter = std::toupper(letter);
  }

  Font *font = &Game::sm_font;
  int size = font->baseSize;

  position = Vector2Add(position, {6, -8});
  position = TextUtils::alignRight(name.c_str(), position, *font, -3, 0);
  DrawTextEx(*font, name.c_str(), position, size, -3, WHITE);
}
