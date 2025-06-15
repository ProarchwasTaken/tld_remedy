#pragma once
#include <raylib.h>
#include "base/entity.h"

namespace CameraUtils {
Camera2D setupField();
Camera2D setupCombat();
void followFieldEntity(Camera2D &camera, Entity *entity);

void combatFollow(Camera2D &camera, float x);
}
