#pragma once
#include <string>
#include <raylib.h>
#include "data/keybinds.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"


class PlayerCmdHud {
public:
  PlayerCmdHud(Vector2 position);
  ~PlayerCmdHud();

  void assign(Mary *player);

  void update();
  void updateDefendText();
  Color determineAttackColor();
  Color determineTechColor(float tech_cost);
  Color determineGSColor();
  Color determineEvadeColor();

  void draw();
  void drawNamePlate(Font *font, int txt_size);
  void drawCmdFrames();
  void drawCmdText(const char *text, int frame, Font *font, int txt_size, 
                   Color color, bool unusable = false);
private:
  Mary *player;
  CombatKeybinds *keybinds;
  SpriteAtlas *atlas;

  std::string player_name;

  Vector2 main_position;
  Color main_color;
  Vector2 name_position;

  Color attack_color = BLACK;

  std::string txt_defend = "--";
  Color defend_color = BLACK;

  Color tech1_color = BLACK;
  Color tech2_color = BLACK;
};
