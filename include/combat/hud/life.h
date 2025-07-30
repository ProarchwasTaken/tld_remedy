#include <raylib.h>
#include <string>
#include "base/party_member.h"
#include "system/sprite_atlas.h"


class LifeHud {
public:
  LifeHud(Vector2 position);
  ~LifeHud();

  void assign(PartyMember *combatant);

  void draw();
  void decideLifeColor();
  void drawLifeBar();
  Rectangle *segmentBlink(float interval);

  void drawLifeText(Font *font, int size);

  static SpriteAtlas atlas;
private:
  PartyMember *user;

  Vector2 main_position;

  Color life_color;
  std::string txt_life;

  bool segment_blink = false;
  float blink_clock = 0.0;
};
