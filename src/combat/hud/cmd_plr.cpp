#include <cassert>
#include <cctype>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/keybinds.h"
#include "scenes/combat.h"
#include "utils/input.h"
#include "utils/text.h"
#include "combat/combatants/party/mary.h"
#include "combat/hud/cmd_plr.h"
#include <plog/Log.h>


PlayerCmdHud::PlayerCmdHud(Vector2 position) {
  main_position = position;
  name_position = Vector2Add(main_position, {66, 1}); 
  main_color = Game::palette[42];

  keybinds = &Game::settings.combat_keybinds;
  this->atlas = &CombatScene::cmd_atlas;
  atlas->use();
}

PlayerCmdHud::~PlayerCmdHud() {
  atlas->release();
}

void PlayerCmdHud::assign(Mary *player) {
  this->player = player;
  if (player == NULL) {
    return;
  }

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

void PlayerCmdHud::update() {
  if (player == NULL) {
    return;
  }

  if (player->state == NEUTRAL || player->canCancel(true)) {
    attack_color = determineAttackColor();
    updateDefendText();
    tech1_color = determineTechColor(player->tech1_cost);
    tech2_color = determineTechColor(player->tech2_cost); 
  }
  else {
    attack_color = Game::palette[2];
    defend_color = Game::palette[2];
    tech1_color = Game::palette[2];
    tech2_color = Game::palette[2];
  }
}

Color PlayerCmdHud::determineAttackColor() {
  if (player->state == ACTION) {
    return Game::palette[51];
  }
  else {
    return WHITE;
  }
}

void PlayerCmdHud::updateDefendText() {
  if (player->critical_life) {
    defend_color = Game::palette[32];
    return;
  }

  if (player->moving) {
    txt_defend = "Ghost Step";
    defend_color = determineGSColor();
  }
  else if (Input::down(keybinds->down, IsGamepadAvailable(0))) {
    txt_defend = "Evade";
    defend_color = determineEvadeColor();
  }
  else {
    txt_defend = "--";
    defend_color = Game::palette[2];
  } 
}

Color PlayerCmdHud::determineTechColor(float tech_cost)
{
  if (player->demoralized) {
    return Game::palette[32];
  }

  if (player->morale < tech_cost) {
    return Game::palette[2];
  }
  else if (player->state == ACTION) {
    return Game::palette[51]; 
  }
  else if (player->morale - (tech_cost * 2) < 0) {
    return Game::palette[26];
  }
  else {
    return WHITE;
  }
}


Color PlayerCmdHud::determineGSColor() {
  bool using_action = player->state == ACTION;
  if (using_action && player->action->id != ActionID::GHOST_STEP) {
    return Game::palette[51];
  }

  float gs_cost = player->gs_cost;
  float threshold = player->max_life * player->LOW_LIFE_THRESHOLD;
  if (player->life - gs_cost <= threshold) {
    return Game::palette[26];
  }
  else {
    return WHITE;
  }
}

Color PlayerCmdHud::determineEvadeColor() {
  bool using_action = player->state == ACTION;
  if (using_action && player->action->id != ActionID::EVADE) {
    return Game::palette[51];
  }
  else if (using_action && player->action->id == ActionID::EVADE) {
    return Game::palette[2];
  }
  else {
    return WHITE;
  }
}

void PlayerCmdHud::draw() {
  if (player == NULL) {
    return;
  }

  Font *font = &Game::sm_font;
  int txt_size = font->baseSize;

  drawNamePlate(font, txt_size);
  drawCmdFrames();

  drawCmdText("Attack", 0, font, txt_size, attack_color);

  drawCmdText(txt_defend.c_str(), 1, font, txt_size, defend_color,
              player->critical_life);

  drawCmdText(player->tech1_name.c_str(), 2, font, txt_size, tech1_color,
              player->demoralized);

  drawCmdText(player->tech2_name.c_str(), 3, font, txt_size, tech2_color,
              player->demoralized);
}

void PlayerCmdHud::drawNamePlate(Font *font, int txt_size) {
  Vector2 position = Vector2Add(main_position, {12, 0});
  Color col_pattern = Game::palette[40];

  DrawTextureRec(atlas->sheet, atlas->sprites[0], position, main_color);
  DrawTextureRec(atlas->sheet, atlas->sprites[1], position, col_pattern);

  const char *name = player_name.c_str();
  DrawTextEx(*font, name, name_position, txt_size, -3, WHITE);
}

void PlayerCmdHud::drawCmdFrames() {
  Vector2 position = Vector2Add(main_position, {0, 11});

  for (int x = 0; x < 4; x++) {
    DrawTextureRec(atlas->sheet, atlas->sprites[2], position, main_color);
    position.y += 11;
  }
}

void PlayerCmdHud::drawCmdText(const char *text, int frame, Font *font, 
                               int txt_size, Color color, bool unusable) 
{
  float offset = 11 * frame;

  Vector2 position = Vector2Add(main_position, {59, 12 + offset});
  position = TextUtils::alignRight(text, position, *font, -3, 0);

  DrawTextEx(*font, text, position, txt_size, -3, color);

  if (unusable) {
    color = Game::palette[33];
    position = Vector2Add(main_position, {0, 11 + offset});

    DrawTextureRec(atlas->sheet, atlas->sprites[3], position, color);
  }
}

