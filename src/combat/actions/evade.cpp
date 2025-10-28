#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/party_member.h"
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "data/damage.h"
#include "data/combat_event.h"
#include "system/sprite_atlas.h"
#include "combat/system/stage.h"
#include "combat/system/evt_handler.h"
#include "combat/actions/evade.h"
#include <plog/Log.h>


Evade::Evade(PartyMember *user, SpriteAtlas &user_atlas, RectEx hitbox,
             EvadeSpriteSet &sprite_set): 
  CombatAction(ActionID::EVADE, ActionType::DEFENSE, user, 
               0.20, 0.40, 0.25) 
{
  name = "Evade";
  this->user = user;

  user->rectExCorrection(hitbox);
  this->hitbox = hitbox;

  this->user_atlas = &user_atlas;
  this->sprite_set = &sprite_set;
  user->sprite = &user_atlas.sprites[sprite_set.id_windup];
}

Evade::~Evade() {
  bool user_cancel = !finished && user->state == CombatantState::ACTION;
  if (!user_cancel) {
    user->intangible = false;
    return;
  }

  ActionID new_action_id = user->action->id;

  if (new_action_id != ActionID::GHOST_STEP) {
    user->intangible = false;
  }
}

void Evade::intercept(DamageData &data) {
  bool valid = data.hitbox != NULL && phase == ActionPhase::ACTIVE;
  if (!valid || !CheckCollisionRecs(hitbox.rect, *data.hitbox)) {
    CombatAction::intercept(data);
    return;
  }

  PLOGI << "Conditions have been met to intercept damage function";
  PLOGD << "Timing: " << act_time * state_clock;
  data.b_def = &user->persist;
  data.def_mod += 0.40;

  float damage = Clamp(user->damageCalculation(data), 0, 9999);
  PLOGD << "Result: " << damage;

  float life_time;
  Color tint;

  if (user->important && state_clock <= 0.25) {
    PLOGI << "Perfect Evasion! Exhaustion depleted!";
    user->sprite = &user_atlas->sprites[sprite_set->id_perfect];
    user->depleteInstant();

    CombatStage::tintStage(Game::palette[2]);
    tint = Game::palette[51];

    Combatant::sfx.play("evade_perfect");
    Game::sleep(0.25);
  }
  else {
    PLOGD << "Redirection damage towards the combatant's exhaustion.";
    user->increaseExhaustion(damage); 
    tint = Game::palette[29];
    Game::sleep(0.05);
  }
 
  state_clock = 0.999999;
  data.intercepted = true;

  user->intangible = true;
  evaded_attack = true;

  CombatHandler::raise<CreateAfterImgCB>(
    CombatEVT::CREATE_AFTERIMAGE, user_atlas, user->sprite,
    user->bounding_box.position, user->direction, 0.25f, tint);
  PLOGI << "Interception complete.";
}

void Evade::windUp() {
  bool end_phase = state_clock >= 1.0;
  if (end_phase) {
    user->sprite = &user_atlas->sprites[sprite_set->id_active];
  }
}

void Evade::action() {
  bool end_phase = state_clock >= 1.0;
  if (!end_phase) {
    return;
  }

  if (evaded_attack) {
    user->sprite = &user_atlas->sprites[sprite_set->id_evade];
    Combatant::sfx.play("evade");
  }
  else {
    user->sprite = &user_atlas->sprites[sprite_set->id_miss];
  }
}

void Evade::endLag() {
  bool end_phase = state_clock >= 1.0;
  if (end_phase) {
    user->intangible = false;
  }
}


void Evade::drawDebug() {
  Color color;
  switch (phase) {
    case ActionPhase::WIND_UP: {
      color = YELLOW;
      break;
    }
    case ActionPhase::ACTIVE: {
      color = RED;
      break;
    }
    case ActionPhase::END_LAG: {
      color = BLUE;
      break;
    }
  }

  color.a = 128;
  DrawRectangleRec(hitbox.rect, color);
}
