#pragma once
#include <raylib.h>
#include <string>
#include "base/party_member.h"
#include "data/technique.h"
#include "system/sprite_atlas.h"


class AssistCmdHud {
public:
  AssistCmdHud(Vector2 position);
  ~AssistCmdHud();

  void assign(PartyMember *companion);

  void update();
  Color decideTechColor(Technique *tech);
  bool isUnusable(Technique *tech);

  void draw();
  void drawNamePlate(Font *font, int txt_size);
  void drawCmdFrames();
  void drawCmdDetails(Technique *tech, int frame, Font *font, 
                      int txt_size, Color color);
  void drawCmdCooldown(Technique *tech, float offset, Font *font, 
                       int txt_size);
private:
  PartyMember *companion;
  SpriteAtlas *atlas;

  Vector2 main_position;
  Color main_color;

  std::string companion_name;
  Vector2 name_position;

  Color tech1_color = BLACK;
  Color tech2_color = BLACK;
};
