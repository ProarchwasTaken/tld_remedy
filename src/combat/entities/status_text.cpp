#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "data/combat_event.h"
#include "data/rect_ex.h"
#include "utils/text.h"
#include "combat/system/evt_handler.h"
#include "combat/entities/status_text.h"
#include <plog/Log.h>

using std::string;


StatusText::StatusText(Combatant *target, string text, Color color) {
  entity_type = EntityType::STATUS_TEXT;
  this->text = text;
  this->color = color;

  setStartPosition(target);
  
  bounding_box.scale = {16, 8};
  bounding_box.offset = {-8, 0};
  rectExCorrection(bounding_box);
  PLOGI << "Entity Created: Status Text [ID: " << entity_id << "]";
}

void StatusText::setStartPosition(Combatant *target) {
  position = target->position;
  position = TextUtils::alignCenter(text.c_str(), position, Game::sm_font,
                                    -3, 0);
}

void StatusText::update() {
  life_clock += Game::deltaTime() / life_time;
  life_clock = Clamp(life_clock, 0.0, 1.0);

  if (life_clock != 1.0) {
    float percentage = 1.0 - life_clock;

    float speed = initial_speed * percentage;
    position.y += speed * Game::deltaTime();

    float alpha = Clamp(percentage / 0.25, 0.0, 1.0);
  }
  else {
    CombatHandler::raise<DeleteEntityCB>(CombatEVT::DELETE_ENTITY,
                                         this->entity_id);
  }
}

void StatusText::draw() {
  Font *font = &Game::sm_font;
  int fnt_size = font->baseSize;

  DrawTextEx(*font, text.c_str(), position, fnt_size, -3, color);
}
