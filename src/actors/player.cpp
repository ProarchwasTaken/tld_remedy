#include <raylib.h>
#include "enums.h"
#include "base/actor.h"
#include "actors/player.h"


PlayerActor::PlayerActor(Vector2 position, enum Direction direction):
Actor("Mary", position, direction)
{
  bounding_box.scale = {32, 32};
  bounding_box.offset = {-16, -28};
  collis_box.scale = {8, 16};
  collis_box.offset = {-4, -12};

  rectExCorrection(bounding_box);
  rectExCorrection(collis_box);
}

void PlayerActor::behavior() {
  
}

void PlayerActor::update() {

}

void PlayerActor::draw() {

}

