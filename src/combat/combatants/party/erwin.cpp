#include <cassert>
#include <random>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/party_member.h"
#include "base/combat_action.h"
#include "data/session.h"
#include "data/animation.h"
#include "utils/animation.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/party/erwin.h"
#include <plog/Log.h>

using std::uniform_real_distribution;
SpriteAtlas Erwin::atlas("combatants", "erwin_combatant");


Erwin::Erwin(Companion *data, Mary *player): 
  PartyMember("Erwin", PartyMemberID::ERWIN, {-96, 152})
{
  this->player = player;

  life = data->life;
  max_life = data->max_life;
  critical_life = life < max_life * LOW_LIFE_THRESHOLD;

  init_morale = data->init_morale;
  morale = init_morale;
  max_morale = data->max_morale;

  offense = data->offense;
  defense = data->defense;
  intimid = data->intimid;
  persist = data->persist;

  afflictPersistent(data->status);

  bounding_box.scale = {64, 64};
  bounding_box.offset = {-32, -64};
  hurtbox.scale = {12, 56};
  hurtbox.offset = {-6, -58};
  rectExCorrection(bounding_box, hurtbox);

  atlas.use();
  sprite = &atlas.sprites[0];
}

Erwin::~Erwin() {
  atlas.release();
}

void Erwin::behavior() {
  tick_clock += Game::deltaTime();
  if (tick_clock >= 1.0) {
    setGoal(ErwinGoals::LOOK_AT_PLR, 0.25);
    tick_clock = 0.0;
  }
}

void Erwin::setGoal(ErwinGoals goal) {
  if (goal > ai_goal) {
    ai_goal = goal;
  }
}

void Erwin::setGoal(ErwinGoals goal, float chance) {
  if (goal < ai_goal) {
    return;
  }

  uniform_real_distribution<float> range(0.0, 1.0);
  float percentage = range(Game::RNG);

  if (percentage <= chance) {
    ai_goal = goal;
  }
}

void Erwin::update() {
  tintFlash();

  switch (state) {
    case CombatantState::NEUTRAL: {
      if (exhaustion != 0) {
        depleteExhaustion();
      }

      neutralLogic();
      break;
    }
    case CombatantState::ACTION: {
      action->logic();
      break;
    }
    case CombatantState::HIT_STUN: {
      stunLogic();
      sprite = getStunSprite();
      break;
    }
    case CombatantState::DEAD: {
      deathLogic();
      break;
    }
  }

  statusLogic();
}

void Erwin::neutralLogic() {
  goalLogic();

  float old_x = position.x;
  movement();

  has_moved = old_x != position.x;
  animationLogic();
}

void Erwin::goalLogic() {
  switch (ai_goal) {
    case ErwinGoals::IDLE: {
      moving_x = 0;
      break;
    }
    case ErwinGoals::LOOK_AT_PLR: {
      lookAtPlayer();
      ai_goal = ErwinGoals::IDLE;
      break;
    }
  }
}

void Erwin::lookAtPlayer() {
  float difference = position.x - player->position.x;
  if (difference > 0) {
    direction = LEFT;
  }
  else {
    direction = RIGHT; 
  }
}

void Erwin::movement() {
  if (moving_x == 0) {
    return;
  }

  direction = static_cast<Direction>(moving_x);

  float speed = default_speed * speed_multiplier;
  float magnitude = speed * direction;

  position.x += magnitude * Game::deltaTime();
}

void Erwin::animationLogic() {
  Animation *next_anim;
  if (has_moved) {
    
    float difference = 1.0 - speed_multiplier;
    float percentage = 1.0 + difference;

    next_anim =  &anim_move;
    next_anim->frame_duration = anim_move_speed * percentage;
    rectExCorrection(bounding_box, hurtbox);
  }
  else {
    next_anim = getIdleAnim();
  }

  SpriteAnimation::play(animation, next_anim, true);
  sprite = &atlas.sprites[*animation->current];
}

Animation *Erwin::getIdleAnim() {
  if (!critical_life) {
    return &anim_idle;
  }
  else {
    return &anim_crit;
  }
}

Rectangle *Erwin::getStunSprite() {
  if (damage_type == DamageType::LIFE) {
    return &atlas.sprites[7];
  }
  else {
    return &atlas.sprites[8];
  }
}

void Erwin::draw() {
  assert(sprite != NULL);

  Rectangle final = *sprite;
  final.width = final.width * direction;

  DrawTexturePro(atlas.sheet, final, bounding_box.rect, {0, 0}, 0, tint);
}

void Erwin::drawDebug() {
  Combatant::drawDebug();

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
