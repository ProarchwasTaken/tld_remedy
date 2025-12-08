#include <cassert>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "enums.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "data/combatant_event.h"
#include "system/sprite_atlas.h"
#include "utils/text.h"
#include "combat/system/cbt_handler.h"
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
  PLOGI << "Assigned enemy hud to player and companion.";
}

void EnemyHud::behavior() {
  EventPool<CombatantEvent> *event_pool = CombatantHandler::get();

  for (auto &event : *event_pool) {
    if (event == nullptr) {
      continue;
    }

    if (event->event_type != CombatantEVT::TOOK_DAMAGE) {
      continue;
    }

    assert(event->sender->entity_type == COMBATANT);
    Combatant *sender = static_cast<Combatant*>(event->sender);
    if (sender->team == CombatantTeam::ENEMY) {
      PLOGI << "Acknowledging TookDamage event sent by '" << sender->name
        << "' [ID: " << sender->entity_id << "]";
      damageHandling(sender);
    }
  }
}

void EnemyHud::damageHandling(Combatant *sender) {
  for (TargetData &data : targets) {
    if (sender == data.target) {
      PLOGD << "Reseting red gauge and end timer.";
      data.red_clock = 0.0;
      data.end_clock = 0.0;
      break;
    }
  }
}

void EnemyHud::update() {
  targetCheck(*player);
  targetCheck(*companion);
  targetLogic();
}

void EnemyHud::targetCheck(PartyMember *member) {
  if (member == NULL) {
    return;
  }

  Combatant *target = member->target;
  if (target == NULL || target->state == DEAD) {
    return;
  }

  TargetData *unused = NULL;
  for (auto &data : targets) {
    if (data.target == NULL) {
      unused = &data;
      continue;
    }

    if (target == data.target) {
      unused = NULL;
      break;
    }
  }

  if (unused != NULL) {
    unused->target = target;
    unused->prev_life = Clamp(target->life, 0.0, target->max_life);
    unused->red_clock = 0.0;
    unused->end_clock = 0.0;
  }
}

void EnemyHud::targetLogic() {
  PartyMember *mary = *player;
  PartyMember *companion = *this->companion;

  for (TargetData &data : targets) {
    Combatant *target = data.target;

    if (target == NULL) {
      continue;
    }

    if (target->life < data.prev_life) {
      redGuageTimer(data);
    }

    bool targeted_by_plr = mary != NULL && target == mary->target;
    bool targeted_by_com = companion != NULL && 
      target == companion->target;

    if (targeted_by_plr || targeted_by_com) {
      data.end_clock = 0.0;
      continue;
    }

    data.end_clock += Game::deltaTime() / END_TIME;
    if (data.end_clock > 1.0) {
      data.target = NULL;
    }
  }
}

void EnemyHud::redGuageTimer(TargetData &data) {
  if (data.target->state != HIT_STUN) {
    data.red_clock += Game::deltaTime() / RED_GAUGE_TIME;
    data.red_clock = Clamp(data.red_clock, 0.0, 1.0);
  }

  if (data.red_clock >= 1.0) {
    Combatant *target = data.target;
    data.prev_life = target->life;
    data.red_clock = 0.0;
  }
}

void EnemyHud::draw() {
  if (*player == NULL) {
    return;
  }

  Vector2 position = main_position;
  for (TargetData &data : targets) {
    if (data.target != NULL) {
      drawTargetHud(data, position);
      position.y += 18;
    }
  }
}

void EnemyHud::drawTargetHud(TargetData &data, Vector2 position) {
  assert(data.target != NULL);
  Combatant *target = data.target;

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
  drawLifeGauge(data, position, gauge_width, max_gauge_width, overflow);

  if (overflow) {
    drawSegments(data, position);
  }

  drawTargetReticle(target, position, frame_width);
}

void EnemyHud::drawLifeGauge(TargetData &data, Vector2 position, float width,
                         float max_width, bool overflow)
{
  if (!overflow) {
    drawGaugeNormal(data, position, width);
  }
  else {
    drawGaugeOverflow(data, position, width, max_width);
  }
}

void EnemyHud::drawGaugeNormal(TargetData &data, Vector2 position,
                               float width) 
{
  Rectangle source = {0, 0, -width, 3};
  Rectangle dest = source;
  dest.x = position.x - 2;
  dest.y = position.y + 2;

  Combatant *target = data.target;
  float max_life = target->max_life;
  float life = Clamp(target->life, 0.0, max_life);

  dest.width = width;
  DrawTexturePro(life_empty, source, dest, {width, 0}, 0, WHITE);

  float percentage;
  float prev_life = data.prev_life;
  if (life < prev_life) {
    percentage = prev_life / max_life;
    dest.width = width * percentage;

    Color color = Game::palette[32];
    color.a = 255 * (1.0 - data.red_clock);

    DrawRectanglePro(dest, {dest.width}, 0, color);
  }

  percentage = life / max_life;

  width = width * percentage;
  source.width = -width;
  dest.width = width;
  DrawTexturePro(life_bar, source, dest, {width, 0}, 0, WHITE);
}

void EnemyHud::drawGaugeOverflow(TargetData &data, Vector2 position,
                                 float width, float max_width) 
{
  Rectangle source = {0, 0, -width, 3};
  Rectangle dest = source;
  dest.x = position.x - 2;
  dest.y = position.y + 2;

  Combatant *target = data.target;
  float max_life = target->max_life;
  float life = Clamp(target->life, 0.0, max_life);

  int segments = std::floorf(life / LIFE_PER_SEGMENT);

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

  float leftover;
  float percentage;
  float prev_life = data.prev_life;

  if (life < prev_life) {
    leftover = prev_life - (LIFE_PER_SEGMENT * segments);
    percentage = Clamp(leftover / LIFE_PER_SEGMENT, 0.0, 1.0);
    dest.width = max_width * percentage;

    Color color = Game::palette[32];
    color.a = 255 * (1.0 - data.red_clock);

    DrawRectanglePro(dest, {dest.width}, 0, color);
  }

  leftover = life - (LIFE_PER_SEGMENT * segments);
  percentage = leftover / LIFE_PER_SEGMENT;

  width = max_width * percentage;
  source.width = -width;
  dest.width = width;
  DrawTexturePro(life_bar, source, dest, {width, 0}, 0, WHITE);
}

void EnemyHud::drawSegments(TargetData &data, Vector2 position) {
  Combatant *target = data.target;
  assert(target->max_life / LIFE_PER_SEGMENT > 1.0);

  float max_life = target->max_life;
  float life = Clamp(target->life, 0.0, max_life);
  float prev_life = data.prev_life;

  int max_segments = std::floorf(max_life / LIFE_PER_SEGMENT);
  int segments = std::floorf(life / LIFE_PER_SEGMENT);
  int prev_segments = std::floorf(prev_life / LIFE_PER_SEGMENT);

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

    if (segments != prev_segments && x >= segments && x < prev_segments) {
      Rectangle rect = {position.x, position.y, 5, 2};
      Color color = Game::palette[32];
      color.a = 255 * (1.0 - data.red_clock);

      DrawRectangleRec(rect, color);
    }
  }
}

void EnemyHud::drawTargetReticle(Combatant *target, Vector2 position, 
                                 float frame_width) 
{ 
  PartyMember *mary = *player;
  PartyMember *companion = *this->companion;

  Rectangle *sprite = &atlas.sprites[4];
  position = Vector2Subtract(position, {frame_width + 10, 1});
  Color color;

  if (mary != NULL && target == mary->target) {
    color = Game::palette[42];
    DrawTextureRec(atlas.sheet, *sprite, position, color);
    position.x -= 10;
  }

  if (companion != NULL && target == companion->target) {
    color = Game::palette[51];
    DrawTextureRec(atlas.sheet, *sprite, position, color);
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
