#include <cassert>
#include <cstddef>
#include <raylib.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/enemy.h"
#include "base/combat_action.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/enemy/servant.h"

SpriteAtlas Servant::atlas("combatants", "servant_combatant");


Servant::Servant(Vector2 position, Direction direction) : 
  Enemy("Servant", EnemyID::SERVANT, position)
{
  max_life = 15;
  life = max_life;

  offense = 8;
  defense = 6;
  intimid = 10;
  persist = 6;

  resilience = 1.0;

  bounding_box.scale = {80, 80};
  bounding_box.offset = {-40, -80};
  hurtbox.scale = {20, 64};
  hurtbox.offset = {-10,-66};

  rectExCorrection(bounding_box, hurtbox);
  atlas.use();

  sprite = &atlas.sprites[0];
}

Servant::~Servant() {
  atlas.release();
}

void Servant::update() {
  switch (state) {
    case CombatantState::NEUTRAL: {
      sprite = &atlas.sprites[0];
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

Rectangle *Servant::getStunSprite() {
  if (damage_type == DamageType::LIFE) {
    return &atlas.sprites[5];
  }
  else {
    return &atlas.sprites[4];
  }
}

void Servant::draw() {
  assert(sprite != NULL);
  Rectangle final = *sprite;
  final.width = final.width * direction;

  applyStaggerEffect(final);
  DrawTexturePro(atlas.sheet, final, bounding_box.rect, {0, 0}, 0, tint);
}

void Servant::drawDebug() {
  Combatant::drawDebug();

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
