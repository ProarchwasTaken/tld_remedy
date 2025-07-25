#include <memory>
#include <raylib.h>
#include "base/enemy.h"
#include "enums.h"
#include "data/rect_ex.h"
#include "base/combat_action.h"
#include "system/sprite_atlas.h"
#include "combat/actions/attack.h"
#include "combat/combatants/enemy/dummy.h"

using std::unique_ptr, std::make_unique;
SpriteAtlas Dummy::atlas("combatants", "dummy");


Dummy::Dummy(Vector2 position, Direction direction) :  
  Enemy("Dummy", EnemyID::DUMMY, position)
{
  max_life = 15;
  life = max_life;

  offense = 9;
  defense = 4;
  intimid = 14;
  persist = 2;

  bounding_box.scale = {64, 64};
  bounding_box.offset = {-32, -64};
  hurtbox.scale = {16, 56};
  hurtbox.offset = {-8, -58};

  rectExCorrection(bounding_box, hurtbox);
  atlas.use();

  sprite = &atlas.sprites[0];
}

Dummy::~Dummy() {
  atlas.release();
}

void Dummy::attack() {
  RectEx hitbox;
  hitbox.scale = {32, 16};
  hitbox.offset = {-16 + (16.0f * direction), -40};

  unique_ptr<CombatAction> action;
  action = make_unique<Attack>(this, atlas, hitbox, atk_set);
  performAction(action);
}

void Dummy::update() {
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
      sprite = &atlas.sprites[1];
      break;
    }
    case CombatantState::DEAD: {
      deathLogic();
      sprite = &atlas.sprites[1];
      break;
    }
  }
}

void Dummy::draw() {
  Rectangle final = *sprite;
  final.width = final.width * direction;

  DrawTexturePro(atlas.sheet, final, bounding_box.rect, {0, 0}, 0, tint);
}

void Dummy::drawDebug() {
  Combatant::drawDebug();

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
