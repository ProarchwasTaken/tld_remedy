#include <cstddef>
#include "enums.h"
#include "base/combatant.h"
#include "base/combat_action.h"
#include "base/enemy.h"
#include "utils/animation.h"
#include "combat/combatants/party/erwin.h"
#include "combat/combatants/party/mary.h"
#include "combat/actions/provoke.h"
#include <plog/Log.h>


Provoke::Provoke(Erwin *user): 
  CombatAction(ActionID::ERWIN_PROVOKE, ActionType::DEFENSE, user, 
               0.10, 1.00, 0.25)
{
  name = "Provoke";
  this->user = user;
  this->atlas = &user->atlas;

  user->intangible = true;
}

Provoke::~Provoke() {
  user->animation = NULL;
}

void Provoke::drawEnemyAggro() {
  Mary *player = user->player;

  if (player->state == DEAD) {
    return;
  }

  int enemy_count = Enemy::memberCount();
  if (enemy_count == 0) {
    return;
  }

  int aggro_drawn = 0;
  int limit = (enemy_count + 1) / 2;
  PLOGD << "Enemies: " << enemy_count << ", Limit: " << limit;

  for (Combatant *combatant : Combatant::existing_combatants) {
    if (aggro_drawn == limit) {
      PLOGI << "Reached the limit of aggro drawn.";
      break;
    }

    if (combatant->state == DEAD) {
      continue;
    }

    if (combatant->team != CombatantTeam::ENEMY) {
      continue;
    }

    if (combatant->target == player) {
      PLOGI << "Attracted the aggro of '" << combatant->name << "' [ID: "
        << combatant->entity_id << "]";
      combatant->target = user;
      aggro_drawn++;
    }
  }
}

void Provoke::windUp() {
  user->sprite = &atlas->sprites[22];

  bool end_state = state_clock == 1.0;
  if (end_state) {
    drawEnemyAggro();
    user->psfx.play("provoke");
  }
}

void Provoke::action() {
  SpriteAnimation::play(user->animation, &anim_provoke, true);
  user->sprite = &atlas->sprites[*user->animation->current];

  bool end_state = state_clock == 1.0;
  if (end_state) {
    user->intangible = false;
  }
}

void Provoke::endLag() {
  user->sprite = &atlas->sprites[22];
}
