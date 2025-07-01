#pragma once
#include <raylib.h>
#include "base/party_member.h"
#include "data/keybinds.h"
#include "data/animation.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "combat/actions/attack.h"


/* Mary is a Combatant whose directly controlled by the player. As such,
 * most of their behavior is determined by user input. This combatant is
 * very important as the the game will end in a fail state if he dies.*/
class Mary : public PartyMember {
public:
  Mary(Player *data);
  ~Mary();
  static void setControllable(bool value);
  static CombatKeybinds key_bind;

  void behavior() override;
  void movementInput(bool gamepad);
  void actionInput(bool gamepad);

  void update() override;
  void neutralLogic();
  void movement();

  Animation *getIdleAnim();
  Rectangle *getStunSprite();

  void draw() override;
  void drawDebug() override;
private:
  const float default_speed = 1.2;
  bool has_moved = false;
  bool moving = true;
  int moving_x = 0;

  float last_moved = 0.0;

  Animation anim_idle = {{0, 1}, 2.0};
  Animation anim_crit = {{2, 3}, 1.0};
  Animation anim_move = {{4, 5, 6, 5}, 0.2};

  AttackAnimSet atk_set = {
    {{9, 10}, 0.05},
    {{10, 9}, 0.05},
    11
  };

  static bool controllable;
  static SpriteAtlas atlas;
};
