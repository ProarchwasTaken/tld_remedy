#include <cctype>
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "scenes/combat.h"
#include "utils/text.h"
#include "combat/combatants/party/mary.h"
#include "combat/hud/cmd_plr.h"
#include <plog/Log.h>


PlayerCmdHud::PlayerCmdHud(Vector2 position) {
  main_position = position;
  name_position = Vector2Add(main_position, {66, 1}); 

  this->atlas = &CombatScene::cmd_atlas;
  atlas->use();
}

PlayerCmdHud::~PlayerCmdHud() {
  atlas->release();
}

void PlayerCmdHud::assign(Mary *player) {
  this->player = player;
  player_name = player->name;

  for (char &letter : player_name) {
    letter = std::toupper(letter);
  }

  Font *font = &Game::sm_font;
  int txt_size = font->baseSize;
  name_position = TextUtils::alignRight(player_name.c_str(), 
                                        name_position, *font, -3, 0);
  PLOGD << "Cmd Hud has been assigned to player.";
}

void PlayerCmdHud::draw() {
  Font *font = &Game::sm_font;
  int txt_size = font->baseSize;

  drawNamePlate(font, txt_size);
}

void PlayerCmdHud::drawNamePlate(Font *font, int txt_size) {
  Vector2 position = Vector2Add(main_position, {12, 0});
  Color col_frame = Game::palette[42];
  Color col_pattern = Game::palette[40];

  DrawTextureRec(atlas->sheet, atlas->sprites[0], position, col_frame);
  DrawTextureRec(atlas->sheet, atlas->sprites[1], position, col_pattern);

  const char *name = player_name.c_str();
  DrawTextEx(*font, name, name_position, txt_size, -3, WHITE);
}
