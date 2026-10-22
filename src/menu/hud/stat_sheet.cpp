#include <array>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "utils/text.h"
#include "menu/hud/stat_sheet.h"

using std::string, std::array;


StatSheet::StatSheet(Vector2 position, Color color) {
  main_position = position;
  this->color = color;
  texture = LoadTexture("graphics/menu/frames/stats_sheet.png");
}

StatSheet::~StatSheet() {
  UnloadTexture(texture);
}

void StatSheet::resetStats() {
  life = 0;
  morale = 0;
  offense = 0;
  defense = 0;
  intimid = 0;
  persist = 0;
  dexterity = 0;
  disipline = 0;
  recovery = 0;
  resilience = 0;
}

void StatSheet::drawFrame() {
  DrawTextureRec(texture, source, main_position, color);
}

void StatSheet::drawStats() {
  array<int, 10> stats = {
    life, morale, offense, defense, intimid, 
    persist, dexterity, disipline, recovery, resilience
  };

  Vector2 slot = {0, 0};
  for (int value : stats) {
    const char *text = TextFormat("%02i", value);
    drawText(text, slot, WHITE);

    if (slot.x == 0) {
      slot.x += 1;
    }
    else {
      slot.y += 1;
      slot.x = 0;
    }
  }
}

void StatSheet::drawStatChanges() {
  array<int, 10> stats = {
    life, morale, offense, defense, intimid, 
    persist, dexterity, disipline, recovery, resilience
  };

  Vector2 slot = {0, 0};
  for (int value : stats) {
    if (value == 0) {
      drawText("--", slot, Game::palette[2]);
    }
    else if (value < 0) {
      const char *text = TextFormat("%i", value);
      drawText(text, slot, Game::palette[34]);
    }
    else {
      const char *text = TextFormat("+%i", value);
      drawText(text, slot, WHITE);
    }

    if (slot.x == 0) {
      slot.x += 1;
    }
    else {
      slot.y += 1;
      slot.x = 0;
    }
  }
}

void StatSheet::drawText(const char *text, Vector2 slot, Color color) {
  Font *font = &Game::sm_font;
  int size = font->baseSize;

  Vector2 base_position = Vector2Add(main_position, {32, 2});
  float spacing_x = 33;
  float spacing_y = 14;

  Vector2 offset = {slot.x * spacing_x, slot.y * spacing_y};
  Vector2 position = Vector2Add(base_position, offset);
  position = TextUtils::alignRight(text, position, *font, -3, 0);

  DrawTextEx(*font, text, position, size, -3, color);
}

