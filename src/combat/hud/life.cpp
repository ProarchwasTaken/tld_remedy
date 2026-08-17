#include <cassert>
#include <cstddef>
#include <memory>
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/combatant_event.h"
#include "base/combatant.h"
#include "base/party_member.h"
#include "utils/math.h"
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
  morale_color = Game::palette[42];

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

  prev_morale = user->morale / user->max_morale;
  white_morale = prev_morale;
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
      StatusID id = effect_gained->effect_id;

      if (id == StatusID::MENDING) {
        PLOGD << "Acknowledging EffectGained event sent by: '" << 
          user->name << "' [ID: " << user->entity_id << "]";
        has_mending = true;
        break;
      }

      if (id == StatusID::DESPONDENT) {
        PLOGD << "Acknowledging EffectGained event sent by: '" << 
          user->name << "' [ID: " << user->entity_id << "]";
        morale_color = Game::palette[37];
        morale_clock = 0.0;
        break;
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
        break;
      }

      if (id == StatusID::DESPONDENT) {
        PLOGD << "Acknowledging EffectGained event sent by: '" << 
          user->name << "' [ID: " << user->entity_id << "]";
        morale_color = Game::palette[42];
        morale_clock = 0.0;
        break;
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
    float morale_lost = event->damage_taken;
    prev_morale = (user->morale + morale_lost) / user->max_morale;
    white_morale = prev_morale;
    dmg_morale_clock = 0.0;

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
    dmg_life_clock += Game::deltaTime() / dmg_life_time;
    dmg_life_clock = Clamp(dmg_life_clock, 0.0, 1.0); 

    float life_percentage = user->life / user->max_life;
    white_life = Math::smoothstep(prev_life, life_percentage, 
                                  dmg_life_clock);
  }

  if (dmg_morale_clock != 1.0) {
    dmg_morale_clock += Game::deltaTime() / dmg_morale_time ;
    dmg_morale_clock = Clamp(dmg_morale_clock, 0.0, 1.0);

    float morale_percentage = user->morale / user->max_morale;
    white_morale = Math::smoothstep(prev_morale, morale_percentage, 
                                    dmg_morale_clock);
  }

  if (user->critical_life) {
    criticalFlash();
  }
  else {
    life_color = Game::palette[32]; 
  }

  if (user->demoralized) {
    demoralizedFlash();
  }
  else {
    morale_color = Game::palette[42];
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

void LifeHud::demoralizedFlash() {
  demo_flash_clock += Game::deltaTime() / demo_flash_time;

  if (demo_flash_clock < 1.0) {
    return;
  }

  demo_flash = !demo_flash;
  demo_flash_clock = 0.0;

  if (demo_flash) {
    morale_color = Game::palette[38];
  }
  else {
    morale_color = Game::palette[37];
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
  drawStatusIcons(position);
  drawLife(position);

  if (user->max_morale > 0) {
    drawMorale(position);
  }

  if (user->tp_threshold > 0) {
    drawTenacity(position);
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

void LifeHud::drawPortrait(Vector2 position) {
  int id = static_cast<int>(user->id);
  Rectangle *sprite = &portrait_atlas.sprites.at(id);
  
  position = Vector2Subtract(position, {27, 41});
  Color tint = user->spriteTint();
  tint.a = 255;

  DrawTextureRec(portrait_atlas.sheet, *sprite, position, tint);
}

void LifeHud::drawStatusIcons(Vector2 position) {
  int count = user->status.size();
  if (count == 0) {
    return;
  }

  Vector2 base_position = Vector2Subtract(position, {19, 5});

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

void LifeHud::drawLife(Vector2 position) {
  DrawTextureRec(atlas.sheet, atlas.sprites[0], position, WHITE);
  drawLifeGauge(position);
}

void LifeHud::drawLifeGauge(Vector2 position) {
  if (has_mending) {
    drawToBeHealed(position);
  }

  if (user->exhaustion != 0) {
    float total = user->life + user->exhaustion;
    float percentage = total / user->max_life;
    percentage = Clamp(percentage, 0.0, 1.0);
    drawGauge(1, position, Game::palette[2], percentage);
  } 

  if (dmg_life_clock != 1.0) {
    drawGauge(1, position, WHITE, white_life);
  }

  if (user->entropy > 0) {
    float percentage = user->life / user->max_life;
    drawGauge(1, position, Game::palette[37], percentage);
  }

  float life_percentage = (user->life - user->entropy) / user->max_life;
  life_percentage = Clamp(life_percentage, 0.0, 1.0);

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

void LifeHud::drawMorale(Vector2 position) {
  position = Vector2Add(position, {10, -6});

  if (morale_clock < 1.0) {
    float percentage = 1.0 - morale_clock;
    float offset = std::sinf(percentage * 5) * 32;
    position.y -= offset * percentage;

    morale_clock += Game::deltaTime() / morale_time;
  }

  DrawTextureRec(atlas.sheet, atlas.sprites[3], position, WHITE);

  if (!user->demoralized) {
    drawMoraleGauge(position);
  }
  else {
    drawDespondenceGauge(position); 
  }
}

void LifeHud::drawMoraleGauge(Vector2 position) {
  if (dmg_morale_clock != 1.0) {
    drawGauge(4, position, WHITE, white_morale, 1.0);
  }

  float morale_percentage = user->morale / user->max_morale;
  morale_percentage = Clamp(morale_percentage, 0.0, 1.0);
  drawGauge(4, position, morale_color, morale_percentage, 1.0);
}

void LifeHud::drawDespondenceGauge(Vector2 position) {
  drawGauge(4, position, Game::palette[36], 1.0, 1.0);

  float start_morale = -user->max_morale;
  float init_morale = user->init_morale;
  float progress = Normalize(user->morale, init_morale, start_morale);
  drawGauge(4, position, morale_color, progress, 1.0);
}

void LifeHud::drawTenacity(Vector2 position) {
  position = Vector2Add(position, {12, 7});
  DrawTextureRec(atlas.sheet, atlas.sprites[5], position, WHITE);

  drawTenacityGauge(position);
}

void LifeHud::drawTenacityGauge(Vector2 position) {
  position.x -= 1;
  float max_tenacity = user->max_life * user->tp_threshold;
  float percentage = user->tenacity / max_tenacity;
  drawGauge(6, position, Game::palette[22], percentage, 1.0);
}

void LifeHud::drawGauge(int index, Vector2 position, Color color,
                        float percentage, float exponent)
{
  Rectangle sprite = atlas.sprites[index];
  float max_width = sprite.width;
  position = Vector2Add(position, {1, 1});

  if (exponent != 1.0) {
    percentage = std::pow(percentage, exponent);
  }

  sprite.width = max_width * percentage;
  DrawTextureRec(atlas.sheet, sprite, position, color);
}
