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
  max_life = 110;
  life = max_life;

  offense = 10;
  defense = 6;
  intimid = 8;
  persist = 2;
  dexterity = 4;
  discipline = 1;

  resilience = 0.8;

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

void Dummy::attackMP() {
  RectEx hitbox;
  hitbox.scale = {32, 16};
  hitbox.offset = {-16 + (16.0f * direction), -40};

  unique_ptr<CombatAction> action;
  action = make_unique<Attack>(this, atlas, hitbox, atk_set);
  performAction(action);
}

void Dummy::attackHP() {
  RectEx hitbox;
  hitbox.scale = {32, 16};
  hitbox.offset = {-16 + (16.0f * direction), -40};

  DamageData data;
  data.damage_type = DamageType::LIFE;
  data.calculation = DamageType::LIFE;
  data.stun_time = 0.5;
  data.stun_type = StunType::NORMAL;
  data.knockback = 20.0;
  data.hit_stop = 0.2;
  data.assailant = this;

  unique_ptr<CombatAction> action;
  action = make_unique<Attack>(this, ActionType::OFFENSE_HP, 0.35, 0.05,
                               0.25, hitbox, data, atlas, atk_set);

  performAction(action);
  sfx.play("enemy_warning1");
}

void Dummy::update() {
  switch (state) {
    case CombatantState::NEUTRAL: {
      sprite = &atlas.sprites[0];
      break;
    }
    case CombatantState::ACTION: {
      action->logic();

      if (kb_push_back) {
        knockbackLogic();
      }
      break;
    }
    case CombatantState::HIT_STUN: {
      stunLogic();
      knockbackLogic();
      sprite = &atlas.sprites[1];
      break;
    }
    case CombatantState::DEAD: {
      deathLogic();
      sprite = &atlas.sprites[1];
      break;
    }
  }

  endLogic();
}

void Dummy::draw() {
  drawSprite(&atlas.sheet);
}

void Dummy::drawDebug() {
  Combatant::drawDebug();

  if (state == CombatantState::ACTION) {
    action->drawDebug();
  }
}
