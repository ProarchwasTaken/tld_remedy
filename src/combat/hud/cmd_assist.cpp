#include <cassert>
#include <cctype>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/party_member.h"
#include "data/technique.h"
#include "utils/text.h"
#include "scenes/combat.h"
#include "combat/hud/cmd_assist.h"
#include <plog/Log.h>


AssistCmdHud::AssistCmdHud(Vector2 position) {
  main_position = position;
  name_position = Vector2Add(main_position, {67, 1});
  main_color = Game::palette[50];

  atlas = &CombatScene::cmd_atlas;
  atlas->use();
}

AssistCmdHud::~AssistCmdHud() {
  atlas->release();
}

void AssistCmdHud::assign(PartyMember *companion) {
  this->companion = companion;
  if (companion == NULL) {
    return;
  }

  assert(companion->id != PartyMemberID::MARY);
  companion_name = companion->name;

  for (char &letter : companion_name) {
    letter = std::toupper(letter);
  }

  Font *font = &Game::sm_font;
  name_position = TextUtils::alignRight(companion_name.c_str(), 
                                        name_position, *font, -3, 0);
  PLOGD << "Cmd Hud has been assigned to companion.";
}

void AssistCmdHud::update() {
  if (companion == NULL || !companion->isEnabled()) {
    return;
  }

  tech1_color = decideTechColor(&companion->tech1);
  tech2_color = decideTechColor(&companion->tech2);
}

Color AssistCmdHud::decideTechColor(Technique *tech) {
  if (companion->demoralized) {
    return Game::palette[32];
  }

  bool morale_cost = tech->type == TechCostType::MORALE;
  float tech_cost = tech->cost;
  if (morale_cost && companion->morale < tech_cost) {
    return Game::palette[2];
  }

  bool one_more_use = companion->morale - (tech_cost * 2) < 0;
  if (morale_cost && one_more_use) {
    return Game::palette[26];
  }

  bool below_one = companion->life <= 1.0;
  if (!morale_cost && below_one) {
    return Game::palette[32]; 
  }

  one_more_use = companion->life - tech_cost <= 1.0;
  if (!morale_cost && one_more_use) { 
    return Game::palette[26];
  }
  else {
    return WHITE;
  }
}

bool AssistCmdHud::isUnusable(Technique *tech) {
  TechCostType cost_type = tech->type;
  if (cost_type == TechCostType::MORALE) {
    return companion->demoralized;
  }
  else {
    return companion->demoralized || companion->life <= 1;
  }
}

void AssistCmdHud::draw() {
  if (companion == NULL) {
    return;
  }

  Font *font = &Game::sm_font;
  int txt_size = font->baseSize;

  drawNamePlate(font, txt_size);
  drawCmdFrames();

  drawCmdDetails(&companion->tech1, 0, font, txt_size, tech1_color);
  drawCmdDetails(&companion->tech2, 1, font, txt_size, tech2_color);
}

void AssistCmdHud::drawNamePlate(Font *font, int txt_size) {
  Vector2 position = Vector2Add(main_position, {12, 0});
  Color col_pattern = Game::palette[49];

  DrawTextureRec(atlas->sheet, atlas->sprites[0], position, main_color);
  DrawTextureRec(atlas->sheet, atlas->sprites[1], position, col_pattern);

  const char *name = companion_name.c_str();
  DrawTextEx(*font, name, name_position, txt_size, -3, WHITE);
}

void AssistCmdHud::drawCmdFrames() {
  Vector2 position = Vector2Add(main_position, {0, 11});

  for (int x = 0; x < 2; x++) {
    DrawTextureRec(atlas->sheet, atlas->sprites[2], position, main_color);
    position.y += 11;
  }
}

void AssistCmdHud::drawCmdDetails(Technique *tech, int frame, Font *font,
                                  int txt_size, Color color)
{
  float offset = 11 * frame;

  bool off_cooldown = tech->clock >= 1.0;
  if (off_cooldown) {
    const char *name = tech->name.c_str();
    Vector2 position = Vector2Add(main_position, {59, 12 + offset});
    position = TextUtils::alignRight(name, position, *font, -3, 0);

    DrawTextEx(*font, name, position, txt_size, -3, color);
  }
  else {
    drawCmdCooldown(tech, offset, font, txt_size);
  }

  bool unusable = isUnusable(tech);
  if (unusable) {
    color = Game::palette[33];
    Vector2 position = Vector2Add(main_position, {0, 11 + offset});

    DrawTextureRec(atlas->sheet, atlas->sprites[3], position, color);
  }
}

void AssistCmdHud::drawCmdCooldown(Technique *tech, float offset, 
                                   Font *font, int txt_size)
{
  float percentage = tech->clock;

  Rectangle source = atlas->sprites[4];
  float max_width = source.width;
  float width = max_width * percentage;
  float remaining = max_width - width;

  source.x += remaining;
  source.width = width;
  Vector2 position = Vector2Add(main_position, 
                                {remaining, 11 + offset}); 
  DrawTextureRec(atlas->sheet, source, position, WHITE);

  float cooldown = companion->calculateCooldown(tech->cooldown);
  float time_remaining = cooldown * (1 - percentage);

  const char *text = TextFormat("%01.01fs", time_remaining);
  position = Vector2Add(main_position, {59, 12 + offset});
  position = TextUtils::alignRight(text, position, *font, -3, 0);

  DrawTextEx(*font, text, position, txt_size, -3, WHITE);
}
