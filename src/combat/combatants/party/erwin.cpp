#include <cassert>
#include <raylib.h>
#include "enums.h"
#include "base/party_member.h"
#include "base/combat_action.h"
#include "data/session.h"
#include "data/animation.h"
#include "utils/animation.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/erwin.h"

SpriteAtlas Erwin::atlas("combatants", "erwin_combatant");


Erwin::Erwin(Companion *com): 
  PartyMember("Erwin", PartyMemberID::ERWIN, {-96, 152})
{
  life = com->life;
  max_life = com->max_life;
  critical_life = life < max_life * LOW_LIFE_THRESHOLD;

  init_morale = com->init_morale;
  morale = init_morale;
  max_morale = com->max_morale;

  offense = com->offense;
  defense = com->defense;
  intimid = com->intimid;
  persist = com->persist;

  afflictPersistent(com->status);

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

}

void Erwin::update() {
  tintFlash();

  switch (state) {
    case CombatantState::NEUTRAL: {
      if (exhaustion != 0) {
        depleteExhaustion();
      }

      Animation *next_anim = getIdleAnim();
      SpriteAnimation::play(animation, next_anim, true);
      sprite = &atlas.sprites[*animation->current];
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
