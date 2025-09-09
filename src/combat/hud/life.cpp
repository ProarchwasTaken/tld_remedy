#include <cassert>
#include <cmath>
#include <memory>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/entity.h"
#include "base/party_member.h"
#include "base/status_effect.h"
#include "data/combatant_event.h"
#include "system/sprite_atlas.h"
#include "utils/text.h"
#include "combat/system/cbt_handler.h"
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

void LifeHud::behavior() {
  EventPool<CombatantEvent> *event_pool = CombatantHandler::get();
  int count = event_pool->size();

  for (int x = 0; x < count; x++) {
    unique_ptr<CombatantEvent> &event = event_pool->at(x);

    if (event == nullptr) {
      continue;
    }

    Entity *sender = event->sender;
    CombatantEVT type = event->event_type;

    if (sender == user && type == CombatantEVT::TOOK_DAMAGE) {
      PLOGD << "Aknowledging TookDamage event sent by: '" << user->name
        << "' [ID: " << user->entity_id << "'";
      
      auto *evt_damage = static_cast<TookDamageCBT*>(event.get());
      damageEventHandling(evt_damage);
      break;
    }
  }
}

void LifeHud::damageEventHandling(TookDamageCBT *event) {
  if (event->damage_type != DamageType::LIFE) {
    return;
  }

  PLOGD << "Initiating damage shake effect.";
  state = SHAKE;
  shake_clock = 0.0;
  hit_timestamp = GetTime();
}

void LifeHud::update() {
  decideLifeColor();
  decideMoraleColor();
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

void LifeHud::decideMoraleColor() {
  for (auto &effect : user->status) {
    if (effect->id == StatusID::DESPONDENT) {
      morale_color = criticalFlash();
      return;
    }
  }

  morale_color = Game::palette[42];
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

  Vector2 position = main_position;

  if (state == SHAKE) {
    float time_elapsed = GetTime() - hit_timestamp;
    float offset = std::sinf(time_elapsed * 50) * 5;
    float percentage = 1.0 - shake_clock;

    position.y += offset * percentage;
    shakeTimer();
  }

  drawBustGraphic(position);
  drawStatusIcons(position);
  drawLife(position, font, txt_size);

  if (user->max_morale != 0) {
    drawMorale(position, font, txt_size);
  }
}

void LifeHud::shakeTimer() {
  shake_clock += Game::deltaTime() / shake_time;

  if (shake_clock >= 1.0) {
    PLOGD << "Ended damage shake effect.";
    state = IDLE;
    shake_clock = 0.0;
  }
}

void LifeHud::drawBustGraphic(Vector2 position) {
  assert(bust_atlas.users() != 0);

  position = Vector2Subtract(position, {26, 49});
  Color tint = user->spriteTint();
  tint.a = 255;
  DrawTextureRec(bust_atlas.sheet, bust_atlas.sprites[0], position, tint);
}

void LifeHud::drawStatusIcons(Vector2 position) {
  int count = user->status.size();
  if (count == 0) {
    return;
  }

  Vector2 base_position = Vector2Subtract(position, {18, 5});

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

void LifeHud::drawLife(Vector2 position, Font *font, int txt_size) {
  DrawTextureRec(atlas.sheet, atlas.sprites[0], position, 
                 life_color);
  drawLifeSegments(position);
  drawLifeText(position, font, txt_size);
}

void LifeHud::drawLifeSegments(Vector2 position) {
  float life_percentage = user->life / user->max_life;

  float combined = user->life + user->exhaustion;
  float with_exhaustion = combined / user->max_life;

  int segments = life_percentage * 10;
  int ex_segments = with_exhaustion * 10;
  float leftover = (life_percentage * 10) - segments;

  position = Vector2Add(position, {7, 2});

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
    else if (segments != ex_segments && x <= ex_segments) {
      sprite = &atlas.sprites[2];
      color = Game::palette[29];
    }
    else {
      sprite = &atlas.sprites[2];
      color = Game::palette[32];
    }

    DrawTextureRec(atlas.sheet, *sprite, position, color);
    position.x += 7;
  }
}

void LifeHud::drawLifeText(Vector2 position, Font *font, int size) {
  float life = std::floorf(user->life);
  txt_life = TextFormat("%02.00f/%02.00f", life, user->max_life);

  position = Vector2Add(position, {78, 3});
  position = TextUtils::alignRight(txt_life.c_str(), position, *font, -3, 
                                   0);

  DrawTextEx(*font, txt_life.c_str(), position, size, -3, life_color);
}

void LifeHud::drawMorale(Vector2 position, Font *font, int txt_size) {
  Vector2 frame_position = Vector2Add(position, {14, -10});
  DrawTextureRec(atlas.sheet, atlas.sprites[3], frame_position, 
                 morale_color);

  drawMoraleGauge(position);
  drawMoraleText(position, font, txt_size);
}

void LifeHud::drawMoraleGauge(Vector2 position) {
  position = Vector2Add(position, {21, -2});
  float morale_percentage = Clamp(user->morale / user->max_morale, 
                                  0.0, 1.0);

  Rectangle gauge = atlas.sprites[4];
  gauge.width = gauge.width * morale_percentage;

  DrawTextureRec(atlas.sheet, atlas.sprites[5], position, WHITE);
  DrawTextureRec(atlas.sheet, gauge, position, morale_color);
}

void LifeHud::drawMoraleText(Vector2 position, Font *font, int size) {
  txt_morale = TextFormat("%02.02f", user->morale);

  position = Vector2Add(position, {92, -12});
  position = TextUtils::alignRight(txt_morale.c_str(), position, *font, 
                                   -3, 0);

  DrawTextEx(*font, txt_morale.c_str(), position, size, -3, morale_color);
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
