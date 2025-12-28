#include <memory>
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

using std::unique_ptr;
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
    end_clock += Game::deltaTime() / end_time;
    end_clock = Clamp(end_clock, 0.0, 1.0);
  }
}

void ComboHud::stunTimer() {
  stun_clock += Game::deltaTime() / stun_time;
  stun_clock = Clamp(stun_clock, 0.0, 1.0);

  if (stun_clock == 1.0) {
    startComboToast();
  }
}

void ComboHud::startComboToast() {
  int combo = Enemy::comboCount();

  if (combo >= 8) {
    CombatHandler::raise<StartToastCB>(CombatEVT::START_TOAST, 4);
    sfx->play("combo_fantastic");
  }
  else if (combo >= 5) {
    CombatHandler::raise<StartToastCB>(CombatEVT::START_TOAST, 3); 
    sfx->play("combo_great");
  }
  else if (combo >= 3) {
    CombatHandler::raise<StartToastCB>(CombatEVT::START_TOAST, 2); 
    sfx->play("combo_good");
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

