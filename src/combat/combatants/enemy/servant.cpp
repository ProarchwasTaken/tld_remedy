#include <cassert>
#include <cstddef>
#include <memory>
#include <raylib.h>
#include "enums.h"
#include "base/combatant.h"
#include "base/enemy.h"
#include "base/combat_action.h"
#include "data/rect_ex.h"
#include "utils/animation.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/enemy/servant.h"

using std::unique_ptr, std::make_unique;
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

void Servant::behavior() {
  Combatant::behavior();

  // Remove this later!
  if (state == NEUTRAL && IsKeyPressed(KEY_SLASH)) {
    attackMP();
  }
}

void Servant::attackMP() {
  RectEx hitbox;
  hitbox.scale = {32, 12};
  hitbox.offset = {-16 + (16.0f * direction), -52};

  unique_ptr<CombatAction> action;
  action = make_unique<Attack>(this, atlas, hitbox, atk_mp_set);
  performAction(action);
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

      SpriteAnimation::play(animation, &anim_dead, false);
      sprite = &atlas.sprites[*animation->current];
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
