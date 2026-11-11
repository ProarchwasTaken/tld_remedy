#include <cassert>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "base/combat_action.h"
#include "data/combat_event.h"
#include "utils/animation.h"
#include "combat/system/evt_handler.h"
#include "combat/combatants/party/mary.h"
#include "combat/sub_weapons/bat.h"
#include "combat/actions/bat_hailmary.h"
#include <plog/Log.h>


BatHailMary::BatHailMary(Mary *user) : 
  CombatAction(ActionID::BAT_HAILMARY, ActionType::OFFENSE_HP, user, 
               0.40, 0.05, 0.0)
{
  name = "Bat Hail Mary";
  this->atlas = &Mary::atlas;
  this->sfx = &Bat::sfx;
  assert(sfx->users() > 0);
}

BatHailMary::~BatHailMary() {
  user->animation = NULL;
}

void BatHailMary::windUp() {
  SpriteAnimation::play(user->animation, &anim_windup, false);
  user->sprite = &atlas->sprites[*user->animation->current];

  bool end_phase = state_clock == 1.0;
  if (end_phase) {
    createProjectile();
    sfx->play("bat_hailmary_throw");
    user->sprite = &atlas->sprites[49];
  }
}

void BatHailMary::createProjectile() {
  PLOGI << "Creating Baseball projectile.";
  int direction = user->direction;
  float offset = 15 * direction;

  Vector2 position = Vector2Add(user->position, {offset, -46});
  CombatHandler::raise<CreateProjectileCB>(CombatEVT::CREATE_PROJECTILE,
                                           ProjectileID::BASEBALL,
                                           position, user);
}

void BatHailMary::action() {

}
