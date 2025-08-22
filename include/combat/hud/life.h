#include <raylib.h>
#include <string>
#include "enums.h"
#include "base/party_member.h"
#include "system/sprite_atlas.h"


class LifeHud {
public:
  LifeHud(Vector2 position);
  ~LifeHud();

  void assign(PartyMember *combatant);

  void update();
  Color criticalFlash();
  void decideLifeColor();
  void decideMoraleColor();

  void draw();

  void drawBustGraphic();
  void drawStatusIcons();
  Rectangle *getIconSprite(StatusID id);

  void drawLife(Font *font, int txt_size);
  void drawLifeText(Font *font, int size);
  void drawLifeSegments();
  Rectangle *segmentBlink(float interval);

  void drawMorale(Font *font, int txt_size);
  void drawMoraleGauge();
  void drawMoraleText(Font *font, int size);


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
};
