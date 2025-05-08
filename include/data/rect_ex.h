#pragma once
#include <raylib.h>


/* Basically an Raylib Rectangle, but with some extra data attached for
 * more complex operations. Like offsetting an rect in relation to an
 * position vector or something.*/
struct RectEx {
  Vector2 scale;
  Vector2 offset;
  Vector2 position;
  Rectangle rect;
};
