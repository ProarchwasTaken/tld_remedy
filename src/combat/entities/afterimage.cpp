#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "data/combat_event.h"
#include "system/sprite_atlas.h"
#include "combat/system/evt_handler.h"
#include "combat/entities/afterimage.h"
#include <plog/Log.h>


AfterImage::AfterImage(SpriteAtlas *atlas, Rectangle *sprite, 
                       Vector2 position, float life_time, Color tint)
{
  entity_type = EntityType::AFTERIMAGE;

  this->atlas = atlas;
  this->sprite = sprite;
  atlas->use();

  this->position = position;
  this->tint = tint;
  tint.a = 180;

  this->life_time = life_time;

  bounding_box.scale = {sprite->width, sprite->height};
  bounding_box.offset = {0, 0};
  rectExCorrection(bounding_box);
  PLOGI << "Entity Created: AfterImage [ID: " << entity_id << "]";
}

AfterImage::~AfterImage() {
  atlas->release();
}

void AfterImage::update() {
  life_clock += Game::deltaTime() / life_time;
  life_clock = Clamp(life_clock, 0.0, 1.0);

  if (life_clock != 1.0) {
    float alpha = Lerp(180, 0, life_clock);
    tint.a = alpha;
  }
  else {
    CombatHandler::raise<DeleteEntityCB>(CombatEVT::DELETE_ENTITY,
                                         this->entity_id);
  }
}

void AfterImage::draw() {
  DrawTextureRec(atlas->sheet, *sprite, position, tint);
}
