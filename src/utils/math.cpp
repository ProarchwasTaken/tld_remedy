#include <raylib.h>
#include <raymath.h>
#include "utils/math.h"


float Math::smoothstep(float start, float end, float amount) {
  float subtrahend = (amount - 1) * (amount - 1);
  float percentage = Lerp(start, end, 1 - subtrahend);
  return percentage;
}
