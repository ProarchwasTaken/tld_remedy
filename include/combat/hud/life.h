#pragma once
#include <memory>
#include <raylib.h>
#include "data/combatant_event.h"
#include "base/party_member.h"


class LifeHud {
public:
  LifeHud(Vector2 position);
  ~LifeHud();

  void assign(PartyMember *user);

  void evaluateEvent(std::unique_ptr<CombatantEvent> &event);
  void damageEventHandling(TookDamageCBT *event);
  void update();
  void criticalFlash();
  void demoralizedFlash();

  void draw();
  void shakeTimer();

  void drawPortrait(Vector2 position);
  void drawStatusIcons(Vector2 position);
  Rectangle *getIconSprite(StatusID id);

  void drawLife(Vector2 position);
  void drawLifeGauge(Vector2 position);
  void drawToBeHealed(Vector2 position);
  void drawLifeText(Vector2 position);
  void drawTenacityText(Vector2 position);

  void drawMorale(Vector2 position);
  void drawMoraleGauge(Vector2 position);
  void drawMoraleText(Vector2 position);

  void drawGauge(int index, Vector2 position, Color color, 
                 float percentage, float exponent = GAUGE_LIFE_EXP);

  static SpriteAtlas atlas;
  static SpriteAtlas status_atlas;
  static SpriteAtlas portrait_atlas;

  static float GAUGE_LIFE_EXP;
private:
  PartyMember *user = NULL;
  Vector2 main_position;
  bool has_mending;

  enum {IDLE, SHAKE} state = IDLE;
  float hit_timestamp = 0;

  float shake_clock = 0.0;
  float shake_time = 0.25;

  Color life_color;
  Color life_txt_color;
  bool crit_flash = false;
  float crit_clock = 0.0;
  float crit_time = 0.20;

  float dmg_life_clock = 1.0;
  float dmg_life_time = 0.50;
  float prev_life;
  float white_life;

  Color morale_color;
  Color morale_txt_color;
  bool demo_flash = false;
  float demo_clock = 0.0;
  float demo_time = 0.25;

  float dmg_morale_clock = 1.0;
  float dmg_morale_time = 0.25;
  float prev_morale;
  float white_morale;
};
