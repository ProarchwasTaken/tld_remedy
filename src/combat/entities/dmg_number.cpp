#include <cassert>
#include <cstddef>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "data/combat_event.h"
#include "data/rect_ex.h"
#include "utils/text.h"
#include "combat/system/evt_handler.h"
#include "combat/entities/dmg_number.h"
#include <plog/Log.h>


DamageNumber::DamageNumber(Combatant *target, DamageType damage_type,
                           float value) 
{
  entity_type = EntityType::DMG_NUMBER;
  this->target = target;
  this->type = damage_type;
  this->value = value;

  setStartPosition(target);
  setTextColor(damage_type);

  bounding_box.scale = {8, 8};
  bounding_box.offset = {-4, 0};
  rectExCorrection(bounding_box);
  PLOGI << "Entity Created: Damage Number [ID: " << entity_id << "]";
}

void DamageNumber::setStartPosition(Combatant *target) {
  RectEx *bounds = &target->bounding_box;

  position = {target->position.x, bounds->position.y};
  position = TextUtils::alignCenter(TextFormat("-%00.01f", value), 
                                    position, Game::sm_font, -3, 0);
}

void DamageNumber::setTextColor(DamageType damage_type) {
  if (damage_type == DamageType::LIFE) {
    color = Game::palette[33];
  }
  else {
    color = Game::palette[42];
  }
}

void DamageNumber::incrementValue(float magnitude) {
  assert(target != NULL);
  PLOGD << "Incrementing existing Damage Number [ID: " << entity_id <<
    "] by: " << magnitude;
  this->value += magnitude;

  setStartPosition(target);
  setTextColor(type);
  life_clock = 0.0;
}

void DamageNumber::update() {
  life_clock += Game::deltaTime() / life_time;
  life_clock = Clamp(life_clock, 0.0, 1.0);

  if (life_clock != 1.0) {
    float percentage = 1.0 - life_clock;

    float speed = initial_speed * percentage;
    position.y -= speed * Game::deltaTime();

    float alpha = Clamp(percentage / 0.25, 0.0, 1.0);
    color.a = 255 * alpha;
  }
  else {
    CombatHandler::raise<DeleteEntityCB>(CombatEVT::DELETE_ENTITY,
                                         this->entity_id);
  }

  if (!visible) {
    visible = true;
  }
}

void DamageNumber::draw() {
  if (!visible) {
    return;
  }

  Font *font = &Game::sm_font;
  int fnt_size = font->baseSize;

  const char *text = TextFormat("-%00.01f", value);
  DrawTextEx(*font, text, position, fnt_size, -3, color);
}
