#pragma once
#include <string>
#include <raylib.h>
#include "base/entity.h"
#include "base/combatant.h"


class StatusText : public Entity {
public:
  StatusText(Combatant *target, std::string text, Color color);
  void setStartPosition(Combatant *target);

  void update() override;
  void draw() override;
private:
  std::string text;
  Color color;

  float initial_speed = 16;
  float life_time = 1.0;
  float life_clock = 0.0;
};
