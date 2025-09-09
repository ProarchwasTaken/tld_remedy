#pragma once
#include <raylib.h>
#include <string>
#include "enums.h"
#include "data/combatant_event.h"
#include "base/party_member.h"
#include "system/sprite_atlas.h"


/* An hud element that's meant to display critical information about the 
 * PartyMember it's assigned to. That includes the PartyMember's Life,
 * active status effects, and Morale if they have any.*/
class LifeHud {
public:
  LifeHud(Vector2 position);
  ~LifeHud();

  void assign(PartyMember *combatant);

  void behavior();
  void damageEventHandling(TookDamageCBT *event);

  void update();
  Color criticalFlash();
  void decideLifeColor();
  void decideMoraleColor();

  void draw();
  void shakeTimer();

  void drawBustGraphic(Vector2 position);
  void drawStatusIcons(Vector2 position);
  Rectangle *getIconSprite(StatusID id);

  void drawLife(Vector2 position, Font *font, int txt_size);
  void drawLifeText(Vector2 position, Font *font, int size);
  void drawLifeSegments(Vector2 position);

  void drawMorale(Vector2 position, Font *font, int txt_size);
  void drawMoraleGauge(Vector2 position);
  void drawMoraleText(Vector2 position, Font *font, int size);

  Rectangle *segmentBlink(float interval);

  static SpriteAtlas atlas;
  static SpriteAtlas bust_atlas;
  static SpriteAtlas status_atlas;
private:
  PartyMember *user;

  Vector2 main_position;

  Color life_color = WHITE;
  std::string txt_life;

  Color morale_color;
  std::string txt_morale;

  bool segment_blink = false;
  float blink_clock = 0.0;

  bool flashed = false;
  float flash_clock = 0.0;

  enum {IDLE, SHAKE} state = IDLE;
  float hit_timestamp = 0;

  float shake_clock = 0.0;
  float shake_time = 0.25;
};
