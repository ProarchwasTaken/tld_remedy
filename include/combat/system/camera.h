#pragma once
#include <raylib.h>
#include "base/party_member.h"


class CombatCamera : public Camera2D {
public:
  CombatCamera();

  void update(PartyMember *player);
  void enemyTargeting(PartyMember *player);
  void follow(float x);

  static Rectangle area;
private:
  float min_length = 1.0;
  float min_speed = 8;
  float fraction_speed = 3.0;
};
