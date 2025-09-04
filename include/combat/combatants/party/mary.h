#pragma once
#include <memory>
#include <raylib.h>
#include "enums.h"
#include "base/party_member.h"
#include "data/keybinds.h"
#include "data/animation.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "combat/actions/attack.h"
#include "combat/actions/ghost_step.h"
#include "combat/actions/evade.h"

class SubWeapon;

enum class MaryAction {
  NONE = -1,
  ATTACK,
  GHOST_STEP,
  EVADE,
  LIGHT_TECH,
  HEAVY_TECH,
};


/* Mary is a Combatant whose directly controlled by the player. As such,
 * most of their behavior is determined by user input. This combatant is
 * very important as the game will end in a fail state if he dies.*/
class Mary : public PartyMember {
public:
  Mary(Player *data);
  ~Mary();

  void assignSubWeapon(SubWeaponID id);
  static void setControllable(bool value);

  void behavior() override;
  void movementInput(bool gamepad);
  void actionInput(bool gamepad);
  void defensiveActionInput(bool gamepad);

  bool canCancel();
  void readActionBuffer();

  void update() override;
  void neutralLogic();
  void bufferTimer();
  void movement();

  Animation *getIdleAnim();
  Rectangle *getStunSprite();

  void draw() override;
  void drawDebug() override;

  static SpriteAtlas atlas;
private:
  static bool controllable;
  CombatKeybinds *keybinds;

  const float default_speed = 68;
  bool has_moved = false;
  bool moving = true;
  int moving_x = 0;

  float last_moved = 0.0;

  MaryAction buffer = MaryAction::NONE;
  float buffer_lifetime = 0.067;
  float buffer_clock = 0.0;

  std::unique_ptr<SubWeapon> sub_weapon;

  Animation anim_idle = {{0, 1}, 2.0};
  Animation anim_crit = {{2, 3}, 1.0};
  float anim_move_speed = 0.2;
  Animation anim_move = {{4, 5, 6, 5}, anim_move_speed};
  Animation anim_dead = {{9, 10}, 0.60};

  AtkAnimSet atk_set = {
    {{11, 12}, 0.05},
    {{12, 11}, 0.05},
    13
  };

  GSSpriteSet gs_set = {14, 8, 15};
  EvadeSpriteSet ev_set = {16, 17, 16, 19, 18};
};
