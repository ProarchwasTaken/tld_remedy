#include <iterator>
#include <raylib.h>
#include <raymath.h>
#include <algorithm>
#include <vector>
#include <cassert>
#include "enums.h"
#include "base/actor.h"
#include "base/combatant.h"
#include "data/line.h"
#include "data/rect_ex.h"
#include "field/system/field_map.h"
#include "utils/collision.h"

using std::vector, std::sort;

bool xCompare(Vector2 &a, Vector2 &b) {
  return a.x > b.x;
}

bool yCompare(Vector2 &a, Vector2 &b) {
  return a.y > a.y;
}


bool Collision::checkX(Actor *actor, float magnitude, int x_direction, 
                       float &collision_x) 
{
  assert(x_direction != 0);

  float half_scale_x = actor->collis_box.scale.x / 2;
  float half_scale_y = actor->collis_box.scale.y / 2;

  float center_x = actor->collis_box.position.x + half_scale_x;
  float offset_x = (half_scale_x + magnitude) * x_direction;

  float center_y = actor->collis_box.position.y + half_scale_y;
  float offset_y = half_scale_y - 1;

  Vector2 start = {center_x, center_y};
  Vector2 end = Vector2Add(start, {offset_x, 0});
  vector<Vector2> collision_points;

  for (int direction = -1; direction < 2; direction++) {
    start.y = center_y + (offset_y * direction);
    end.y = start.y;

    Vector2 point;
    for (Line line : FieldMap::collision_lines) {
      if (CheckCollisionLines(start, end, line.start, line.end, &point)) {
        collision_points.push_back(point);
        break;
      }
    }
  }

  if (collision_points.empty()) {
    return false;
  }

  vector<Vector2>::iterator viable_point;
  sort(collision_points.begin(), collision_points.end(), xCompare);

  if (x_direction == 1) {
    viable_point = collision_points.begin();
  } 
  else if (x_direction == -1) {
    viable_point = std::prev(collision_points.end());
  }

  collision_x = viable_point->x;
  return true;
}

bool Collision::checkY(Actor *actor, float magnitude, int y_direction, 
                       float &collision_y)
{
  assert(y_direction != 0);

  float half_scale_x = actor->collis_box.scale.x / 2;
  float half_scale_y = actor->collis_box.scale.y / 2;

  float center_x = actor->collis_box.position.x + half_scale_x;
  float offset_x = half_scale_x - 1;

  float center_y = actor->collis_box.position.y + half_scale_y;
  float offset_y = (half_scale_y + magnitude) * y_direction;

  Vector2 start = {center_x, center_y};
  Vector2 end = Vector2Add(start, {0, offset_y});
  vector<Vector2> collision_points;

  for (int direction = -1; direction < 2; direction++) {
    start.x = center_x + (offset_x * direction);
    end.x = start.x;

    Vector2 point;
    for (Line line : FieldMap::collision_lines) {
      if (CheckCollisionLines(start, end, line.start, line.end, &point)) {
        collision_points.push_back(point);
        break;
      }
    }
  }

  if (collision_points.empty()) {
    return false;
  }

  vector<Vector2>::iterator viable_point;
  sort(collision_points.begin(), collision_points.end(), yCompare);

  if (y_direction == 1) {
    viable_point = collision_points.begin();
  }
  else if (y_direction == -1) {
    viable_point = std::prev(collision_points.end());
  }

  collision_y = viable_point->y;
  return true;
}

void Collision::snapX(Actor *actor, float x, int x_direction) {
  RectEx *collis_box = &actor->collis_box;

  float half_scale_x = collis_box->scale.x / 2;
  float collis_x = collis_box->position.x + half_scale_x;
  collis_x += half_scale_x * x_direction;

  float difference = actor->position.x - collis_x;
  actor->position.x = x + difference;
}

void Collision::snapY(Actor *actor, float y, int y_direction) {
  RectEx *collis_box = &actor->collis_box;

  float half_scale_y = collis_box->scale.y / 2;
  float collis_y = collis_box->position.y + half_scale_y;
  collis_y += half_scale_y * y_direction;

  float difference = actor->position.y - collis_y;
  actor->position.y = y + difference; 
}

int Collision::checkX(Combatant *combatant, float magnitude, 
                      Direction direction) 
{
  RectEx *hurtbox = &combatant->hurtbox;

  float half_scale = hurtbox->scale.x / 2;
  float offset = (magnitude + half_scale) * direction;
  float bounds = 512;

  float x = combatant->position.x;
  if (x + offset > bounds) {
    return 1;
  }
  else if (x + offset < -bounds) {
    return -1;
  }
  else {
    return 0;
  }
}

void Collision::snapX(Combatant *combatant, Direction direction) {
  RectEx *hurtbox = &combatant->hurtbox;
  float half_scale = hurtbox->scale.x / 2;
  float bounds = 512;

  combatant->position.x = (bounds - half_scale) * direction;
}
