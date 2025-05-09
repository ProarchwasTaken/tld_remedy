#include <iterator>
#include <raylib.h>
#include <raymath.h>
#include <algorithm>
#include <vector>
#include <cassert>
#include "base/actor.h"
#include "data/line.h"
#include "system/field_map.h"
#include "utils/collision.h"

using std::vector, std::sort;

bool xCompare(Vector2 &a, Vector2 &b) {
  return a.x < b.x;
}

bool yCompare(Vector2 &a, Vector2 &b) {
  return a.y < a.y;
}


bool Collision::checkX(Actor *actor, float magnitude, int x_direction, 
                       float &collision_x) 
{
  assert(x_direction != 0);

  float half_scale_x = actor->collis_box.scale.x / 2;
  float half_scale_y = actor->collis_box.scale.y / 2;

  float center_x = actor->collis_box.position.x + half_scale_x;
  float offset_x = half_scale_x * x_direction;

  float y = actor->collis_box.position.y;

  vector<Vector2> collision_points;

  Vector2 start = {center_x + offset_x, y};
  Vector2 end = start;
  end.x += magnitude * x_direction;

  for (int iter = 0; iter < 3; iter++) {
    Vector2 point;
    for (Line line : FieldMap::collision_lines) {
      if (CheckCollisionLines(start, end, line.start, line.end, &point)) {
        collision_points.push_back(point);
        break;
      }
    }

    start.y += half_scale_y;
    end.y += half_scale_y;
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

  float x = actor->collis_box.position.x;

  float center_y = actor->collis_box.position.y + half_scale_y;
  float offset_y = half_scale_y * y_direction;

  vector<Vector2> collision_points;

  Vector2 start = {x, center_y + offset_y};
  Vector2 end = start;
  end.y += magnitude * y_direction;

  for (int iter = 0; iter < 3; iter++) {
    Vector2 point;
    for (Line line : FieldMap::collision_lines) {
      if (CheckCollisionLines(start, end, line.start, line.end, &point)) {
        collision_points.push_back(point);
        break;
      }
    }

    start.x += half_scale_x;
    end.x += half_scale_x;
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
