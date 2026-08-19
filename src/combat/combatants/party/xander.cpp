#include <cassert>
#include "base/combatant.h"
#include "enums.h"
#include "base/party_member.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "combat/combatants/party/mary.h"
#include "combat/combatants/party/xander.h"
#include <plog/Log.h>

SpriteAtlas Xander::atlas("combatants", "xander_combatant");


Xander::Xander(Companion *data, Mary *player) : 
  PartyMember("Xander", PartyMemberID::XANDER, {-96, 152}, &atlas)
{
  assert(id == data->member_id);
  this->player = player;

  life = data->life;
  max_life = data->max_life;
  critical_life = life < max_life * LOW_LIFE_THRESHOLD;

  init_morale = data->init_morale;
  morale = init_morale;
  max_morale = data->max_morale;

  tenacity = 6;
  tp_natural = 0.20;
  tp_threshold = tp_natural;

  offense = data->offense;
  defense = data->defense;
  intimid = data->intimid;
  persist = data->persist;
  dexterity = data->dexterity;
  discipline = data->discipline;

  recovery = data->recovery;
  resilience = data->resilience;

  tech1 = {"Tail Whip", TechCostType::LIFE, 2.0};
  tech1.cooldown = 5.0;

  tech2 = {"Steel Wall", TechCostType::LIFE, 0.0};
  tech2.cooldown = 8.0;

  afflictPersistent(data->status);

  bounding_box.scale = {144, 128};
  bounding_box.offset = {-72, -128};
  hurtbox.scale = {48, 78};
  hurtbox.offset = {-24, -80};
  rectExCorrection(bounding_box, hurtbox);

  atlas.use();
  sprite = &atlas.sprites[0];
}

Xander::~Xander() {
  atlas.release();
}

void Xander::update() {
  tintFlash();

  switch (state) {
    case CombatantState::NEUTRAL: {
      if (exhaustion != 0) {
        depleteExhaustion();
      }

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
      break;
    }
    case CombatantState::DEAD: {
      deathLogic();
      break;
    }
  }

  endLogic();
}

void Xander::draw() {
  drawSprite(&atlas.sheet);
}
