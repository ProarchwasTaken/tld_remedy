#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include "base/party_member.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"
#include "combat/hud/enemy.h"
#include <plog/Log.h>

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
    drawLifeGauge(mary->target);
  }
}

void EnemyHud::drawLifeGauge(Combatant *target) {
  float max_gauge_width = 136;

  float width_percentage = target->max_life / 20;
  float gauge_width = max_gauge_width * width_percentage;
  float frame_width = gauge_width + 4;

  Rectangle frame = {main_position.x, main_position.y, frame_width, 7};
  DrawRectanglePro(frame, {frame_width, 0}, 0, BLACK);

  Rectangle source = {0, 0, -gauge_width, 3};
  Rectangle dest = source;
  dest.x = main_position.x - 2;
  dest.y = main_position.y + 2;
  dest.width = gauge_width;

  DrawTexturePro(life_empty, source, dest, {gauge_width, 0}, 0, WHITE);

  float life_percentage = target->life / target->max_life;

  gauge_width = gauge_width * life_percentage;
  source.width = -gauge_width;
  dest.width = gauge_width;
  DrawTexturePro(life_bar, source, dest, {gauge_width, 0}, 0, WHITE);
}
