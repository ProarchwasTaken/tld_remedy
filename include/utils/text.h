#pragma once
#include <raylib.h>


namespace TextUtils {
Vector2 alignLeft(const char *text, Vector2 position, Font &font, 
                  int spacing, int alignment, float multiplier = 1);
Vector2 alignCenter(const char *text, Vector2 position, Font &font, 
                    int spacing, int alignment, float multiplier = 1);
Vector2 alignRight(const char *text, Vector2 position, Font &font,
                   int spacing, int alignment, float multiplier = 1);
}
