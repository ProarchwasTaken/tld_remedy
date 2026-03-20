#include <cassert>
#include <cstddef>
#include <memory>
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "data/combatant_event.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "utils/math.h"
#include "utils/text.h"
#include "combat/status_effects/mending.h"
#include "combat/hud/life.h"
#include <plog/Log.h>

using std::unique_ptr;
SpriteAtlas LifeHud::atlas("hud", "hud_life");
SpriteAtlas LifeHud::status_atlas("hud", "status_icons");
SpriteAtlas LifeHud::portrait_atlas("hud", "combat_portraits");

float LifeHud::GAUGE_LIFE_EXP = std::logf(11.0 / 82) / std::logf(0.3);


LifeHud::LifeHud(Vector2 position) {
  main_position = position;
  life_color = Game::palette[32];
  atlas.use();
  status_atlas.use();
  portrait_atlas.use();
}

LifeHud::~LifeHud() {
  atlas.release();
  status_atlas.release();
  portrait_atlas.release();
}

void LifeHud::assign(PartyMember *user) {
  this->user = user;

  if (user == NULL) {
    return;
  } 

  PLOGD << "Lifehud instance assigned to Combatant: '" 
    << user->name << "'";
  prev_life = user->life / user->max_life;
  white_life = prev_life;
}

void LifeHud::evaluateEvent(unique_ptr<CombatantEvent> &event) {
  if (user == NULL) {
    return;
  }

  Entity *sender = event->sender;
  if (sender != user) {
    return;
  }

  switch (event->event_type) {
    case CombatantEVT::TOOK_DAMAGE: {
      PLOGD << "Acknowledging TookDamage event sent by: '" << user->name
        << "' [ID: " << user->entity_id << "]";
      
      auto *evt_damage = static_cast<TookDamageCBT*>(event.get());
      damageEventHandling(evt_damage);
      break;
    }
    case CombatantEVT::EFFECT_GAINED: {
      auto *effect_gained = static_cast<EffectGainedCBT*>(event.get());
      if (effect_gained->effect_id == StatusID::MENDING) {
        PLOGD << "Acknowledging EffectGained event sent by: '" << 
          user->name << "' [ID: " << user->entity_id << "]";
        has_mending = true;
      }

      break;
    }
    case CombatantEVT::EFFECT_LOST: {
      auto *effect_lost = static_cast<EffectLostCBT*>(event.get());
      StatusID id = effect_lost->effect_id;
      if (id == StatusID::MENDING) {
        PLOGD << "Acknowledging EffectLost event sent by: '" << 
          user->name << "' [ID: " << user->entity_id << "]";
        has_mending = false;
      }

      break;
    }
    default: {

    }
  }
}

void LifeHud::damageEventHandling(TookDamageCBT *event) {
  if (event->damage_type == DamageType::LIFE) {
    float life_lost = event->damage_taken;
    prev_life = (user->life + life_lost) / user->max_life;
    white_life = prev_life;
    dmg_life_clock = 0.0;

    shake_time = 0.25;
  }
  else {
    shake_time = 0.10;
  }

  bool in_hitstun = event->resulting_state == HIT_STUN;
  if (in_hitstun && event->stun_type == StunType::STAGGER) {
    shake_time = 0.5;
  }

  PLOGD << "Initiating damage shake effect.";
  state = SHAKE;
  shake_clock = 0.0;
  hit_timestamp = GetTime();
}

void LifeHud::update() {
  if (user == NULL) {
    return;
  } 

  if (dmg_life_clock != 1.0) {
    PLOGI << "This is running.";
    dmg_life_clock += Game::deltaTime() / dm_life_time;
    dmg_life_clock = Clamp(dmg_life_clock, 0.0, 1.0); 

    float life_percentage = user->life / user->max_life;
    white_life = Math::smoothstep(prev_life, life_percentage, 
                                  dmg_life_clock);
  }

  if (user->critical_life) {
    criticalFlash();
  }
  else {
    life_color = Game::palette[32]; 
  }
}

void LifeHud::criticalFlash() {
  crit_clock += Game::deltaTime() / crit_time;

  if (crit_clock < 1.0) {
    return;
  }

  crit_flash = !crit_flash;
  crit_clock = 0.0;

  if (crit_flash) {
    life_color = Game::palette[34];
  } 
  else {
    life_color = Game::palette[32];
  }
}

void LifeHud::draw() {
  if (user == NULL) {
    return;
  }

  Vector2 position = main_position;

  if (state == SHAKE) {
    float time_elapsed = GetTime() - hit_timestamp;
    float offset = std::sinf(time_elapsed * 50) * 5;
    float percentage = 1.0 - shake_clock;

    position.y += offset * percentage;
    shakeTimer();
  }

  drawPortrait(position);
  drawLife(position);
}

void LifeHud::shakeTimer() {
  shake_clock += Game::deltaTime() / shake_time;

  if (shake_clock >= 1.0) {
    PLOGD << "Ended damage shake effect.";
    state = IDLE;
    shake_clock = 0.0;
  }
}

void LifeHud::drawPortrait(Vector2 position) {
  int id = static_cast<int>(user->id);
  Rectangle *sprite = &portrait_atlas.sprites.at(id);
  
  position = Vector2Subtract(position, {27, 41});
  Color tint = user->spriteTint();
  tint.a = 255;

  DrawTextureRec(portrait_atlas.sheet, *sprite, position, tint);
}

void LifeHud::drawLife(Vector2 position) {
  DrawTextureRec(atlas.sheet, atlas.sprites[0], position, WHITE);
  drawLifeGauge(position);
  drawLifeText(position);

  if (user->tenacity != 0.0) {
    drawTenacityText(position);
  }
}

void LifeHud::drawLifeGauge(Vector2 position) {
  if (has_mending) {
    drawToBeHealed(position);
  }

  if (user->life != user->max_life && user->tenacity != 0) {
    float life_total = user->life + user->exhaustion;
    float percentage = (life_total + user->tenacity) / user->max_life;
    percentage = Clamp(percentage, 0.0, 1.0);
    drawGauge(2, position, life_color, percentage);
  }

  if (user->exhaustion != 0) {
    float percentage = (user->life + user->exhaustion) / user->max_life;
    percentage = Clamp(percentage, 0.0, 1.0);
    drawGauge(1, position, Game::palette[29], percentage);
  }

  float life_percentage = user->life / user->max_life;
  life_percentage = Clamp(life_percentage, 0.0, 1.0);

  if (dmg_life_clock != 1.0) {
    drawGauge(1, position, WHITE, white_life);
  }

  drawGauge(1, position, life_color, life_percentage);
}

void LifeHud::drawToBeHealed(Vector2 position) {
  Mending *mending = NULL;
  for (auto &effect : user->status) {
    if (effect->id == StatusID::MENDING) {
      mending = static_cast<Mending*>(effect.get());
      break;
    }
  }

  if (mending == NULL) {
    return;
  }

  float life_total = user->life + user->exhaustion + user->tenacity;
  float to_be_healed = mending->to_be_healed;

  float percentage = (life_total + to_be_healed) / user->max_life;
  percentage = Clamp(percentage, 0.0, 1.0);
  drawGauge(1, position, Game::palette[2], percentage);
}

void LifeHud::drawLifeText(Vector2 position) {
  Font *font = &Game::sm_font;
  int size = font->baseSize;
  Color color = Game::palette[34];

  const char *text = TextFormat("%02.00f/%02.00f", user->life, 
                                user->max_life);
  position = Vector2Add(position, {82, 4});
  position = TextUtils::alignRight(text, position, *font, -3, 0);

  DrawTextEx(*font, text, position, size, -3, color);
}

void LifeHud::drawTenacityText(Vector2 position) {
  Font *font = &Game::sm_font;
  int size = font->baseSize;
  Color color = Game::palette[22];

  const char *text = TextFormat("+%01.00f", user->tenacity);
  position = Vector2Add(position, {82, 4});
  DrawTextEx(*font, text, position, size, -3, color);
}

void LifeHud::drawGauge(int index, Vector2 position, Color color,
                        float percentage, float exponent)
{
  Rectangle sprite = atlas.sprites[index];
  float max_width = sprite.width;
  position = Vector2Add(position, {1, 1});

  percentage = std::pow(percentage, exponent);

  sprite.width = max_width * percentage;
  DrawTextureRec(atlas.sheet, sprite, position, color);
}
