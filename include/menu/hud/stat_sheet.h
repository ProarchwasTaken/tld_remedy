#pragma once
#include <raylib.h>


class StatSheet {
public:
  StatSheet(Vector2 position, Color color);
  ~StatSheet();

  void resetStats();

  void drawFrame();
  void drawStats();
  void drawStatChanges();
  void drawText(const char *text, Vector2 slot, Color color);

  int life = 0;
  int morale = 0;
  int offense = 0;
  int defense = 0;
  int intimid = 0;
  int persist = 0;
  int dexterity = 0;
  int disipline = 0;
  int recovery = 0;
  int resilience = 0;

  Rectangle source = {0, 0, 70, 74};
private:
  Texture texture;
  Vector2 main_position;
  Color color = WHITE;
};
