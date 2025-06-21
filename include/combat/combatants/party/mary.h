#pragma once
#include "base/party_member.h"
#include "data/keybinds.h"
#include "data/session.h"


class Mary : public PartyMember {
public:
  Mary(Player *data);
  static void setControllable(bool value);
  static CombatKeybinds key_bind;

  void behavior() override;
  void movementInput(bool gamepad);
  void actionInput(bool gamepad);

  void update() override;
  void neutralLogic();
  void movement();

  void draw() override;
  void drawDebug() override;
private:
  const float default_speed = 1.2;
  bool has_moved = false;
  bool moving = true;
  int moving_x = 0;

  static bool controllable;
};
