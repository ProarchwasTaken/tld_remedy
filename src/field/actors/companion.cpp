#include <raylib.h>
#include "enums.h"
#include "base/actor.h"
#include "field/actors/companion.h"


CompanionActor::CompanionActor(Vector2 position, 
                               enum Direction direction):
Actor("Companion", ActorType::COMPANION, position, direction)
{
  bounding_box.scale = {32, 32};
  bounding_box.offset = {-16, -28};
  collis_box.scale = {8, 16};
  collis_box.offset = {-4, -12};

  rectExCorrection(bounding_box, collis_box);
}

void CompanionActor::behavior() {

}

void CompanionActor::update() {

}

void CompanionActor::draw() {

}

