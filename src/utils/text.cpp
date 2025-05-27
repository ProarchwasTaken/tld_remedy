#include <cmath>
#include <raylib.h>
#include <plog/Log.h>
#include "utils/text.h"


Vector2 TextUtils::alignLeft(const char *text, Vector2 position, 
                             Font &font, int spacing, int alignment, 
                             float multiplier)
{
  if (alignment == 0) {
    PLOGW << "Having alignment at 0 is kind of redundant.";
    return position;
  }

  int font_size = font.baseSize * multiplier;

  Vector2 scale = MeasureTextEx(font, text, font_size, spacing);
  float offset = (scale.y / 2) * alignment;

  position.y = std::roundf(position.y - offset);
  return position;
}

Vector2 TextUtils::alignCenter(const char *text, Vector2 position, 
                               Font &font, int spacing, int alignment,
                               float multiplier)
{
  int font_size = font.baseSize * multiplier;

  Vector2 scale = MeasureTextEx(font, text, font_size, spacing);

  float x_offset = (scale.x / 2);
  float y_offset = (scale.y / 2) * alignment;

  position.x = std::roundf(position.x - x_offset);
  position.y = std::roundf(position.y - y_offset);
  return position;
}

Vector2 TextUtils::alignRight(const char *text, Vector2 position, 
                              Font &font, int spacing, int alignment,
                              float multiplier)
{
  int font_size = font.baseSize * multiplier;

  Vector2 scale = MeasureTextEx(font, text, font_size, spacing);

  float y_offset = (scale.y / 2) * alignment;

  position.x = position.x - scale.x;
  position.y = std::roundf(position.y - y_offset);
  return position;
}
