#pragma once
#include <raylib.h>
#include "base/entity.h"

namespace CameraUtils {
Camera2D setupField();
void followFieldEntity(Camera2D &camera, Entity *entity);
}
