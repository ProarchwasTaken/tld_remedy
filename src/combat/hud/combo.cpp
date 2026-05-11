#include <memory>
#include <string>
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/enemy.h"
#include "data/combat_event.h"
#include "data/combatant_event.h"
#include "utils/text.h"
#include "system/sprite_atlas.h"
#include "combat/system/evt_handler.h"
#include "combat/hud/combo.h"

using std::unique_ptr, std::string;
SpriteAtlas ComboHud::atlas("hud", "hud_combo");


ComboHud::ComboHud(Vector2 position) {
  main_position = position;
  atlas.use();

  sfx = &Combatant::sfx;
  sfx->use();
}

ComboHud::~ComboHud() {
  atlas.release();
  sfx->release();
}

void ComboHud::evaluateEvent(unique_ptr<CombatantEvent> &event) {
  if (event->event_type != CombatantEVT::TOOK_DAMAGE) {
    return;
  }

  TookDamageCBT *dmg_event = static_cast<TookDamageCBT*>(event.get());
  Combatant *victim = static_cast<Combatant*>(dmg_event->sender);

  bool from_enemy = victim->team == CombatantTeam::ENEMY;
  if (!from_enemy) {
    return;
  }

  if (damage_dealt != damage_total) {
    damage_total = 0;
  }
  if (dmg_event->damage_type == DamageType::LIFE) {
    damage_dealt += dmg_event->damage_taken;
    damage_total = damage_dealt;
  }

  hit_clock = 0.0;

  if (dmg_event->stun_time > 0) {
    stun_time = dmg_event->stun_time;
    stun_clock = 0.0;
    end_clock = 0.0;
  }
}

void ComboHud::update() {
  if (stun_clock != 1.0) {
    stunTimer();
  }
  else if (end_clock != 1.0) {
    endTimer();
  }

  if (highest_combo > 0) { 
    highestComboDecay();
  }
}

void ComboHud::stunTimer() {
  stun_clock += Game::deltaTime() / stun_time;
  stun_clock = Clamp(stun_clock, 0.0, 1.0);

  if (stun_clock == 1.0) {
    damage_dealt = 0;
    startComboToast();
  }
}

void ComboHud::endTimer() {
  end_clock += Game::deltaTime() / end_time;
  end_clock = Clamp(end_clock, 0.0, 1.0);

  if (end_clock == 1.0) {
    damage_total = 0;
  }
}

void ComboHud::highestComboDecay() {
  decay_clock += Game::deltaTime() / decay_time;

  if (decay_clock >= 1.0) {
    highest_combo = 0;
    decay_clock = 0;
  }
}

void ComboHud::startComboToast() {
  int combo = Enemy::comboCount();

  string sound_name;
  if (combo >= 8) {
    CombatHandler::raise<StartToastCB>(CombatEVT::START_TOAST, 4);
    sound_name = "combo_fantastic";
  }
  else if (combo >= 5) {
    CombatHandler::raise<StartToastCB>(CombatEVT::START_TOAST, 3); 
    sound_name = "combo_great";
  }
  else if (combo >= 3) {
    CombatHandler::raise<StartToastCB>(CombatEVT::START_TOAST, 2); 
    sound_name = "combo_good";
  }

  if (!sound_name.empty() && combo > highest_combo) {
    highest_combo = combo;
    sfx->play(sound_name);
  }
}

void ComboHud::draw() {
  int combo = Enemy::comboCount();
  if (combo <= 1 || end_clock == 1.0) {
    return;
  }

  float unflipped = Clamp((-0.75 + end_clock) / 0.25, 0.0, 1.0);
  float alpha_percentage = 1.0 - unflipped;

  tint.a = 255 * alpha_percentage;

  DrawTextureRec(atlas.sheet, atlas.sprites[0], main_position, tint);

  Rectangle sprite = atlas.sprites[1];
  float percentage = 1.0 - stun_clock;
  sprite.width = sprite.width * percentage;
  Vector2 position = Vector2Add(main_position, {1, 1});

  DrawTextureRec(atlas.sheet, sprite, position, tint);
  drawComboCount(combo);
  drawTotalDamage();
}

void ComboHud::drawComboCount(int value) {
  if (hit_clock != 1.0) {
    hit_clock += Game::deltaTime() / hit_time;
    hit_clock = Clamp(hit_clock, 0.0, 1.0);
  }

  Font *font = &Game::med_font;
  float multiplier = 1.0 + (1.0 - hit_clock);
  int txt_size = font->baseSize * multiplier;

  const char *text = TextFormat("%02i", value); 
  Vector2 position = Vector2Add(main_position, {17, 2});
  position = TextUtils::alignRight(text, position, *font, -2, 0,
                                   multiplier);

  DrawTextEx(*font, text, position, txt_size, -2, tint);
}

void ComboHud::drawTotalDamage() {
  if (damage_total == 0) {
    return;
  }

  float x_offset = 0;
  float y_offset = 0;

  if (hit_clock != 1.0) {
    float time_elapsed = hit_time * hit_clock;
    float percentage = 1.0 - hit_clock;

    x_offset = std::cosf(time_elapsed * 50) * 3;
    x_offset = x_offset * percentage;

    y_offset = std::sinf(time_elapsed * 30) * 4;
    y_offset = y_offset * percentage;
  }

  Vector2 offset = {35 + x_offset, 13 + y_offset};
  Vector2 position = Vector2Add(main_position, offset);

  Rectangle *sprite = &atlas.sprites[2];
  DrawTextureRec(atlas.sheet, *sprite, position, tint);

  Font *font = &Game::sm_font;
  int txt_size = font->baseSize;

  Color txt_color;
  if (stun_clock == 1.0) {
    txt_color = Game::palette[22];
    txt_color.a = tint.a;
  }
  else {
    txt_color = tint;
  }

  const char *text = TextFormat("%02.00f", damage_total);
  position = Vector2Subtract(position, {4, 2});
  position = TextUtils::alignRight(text, position, *font, -3, 0);

  DrawTextEx(*font, text, position, txt_size, -3, txt_color);
}

