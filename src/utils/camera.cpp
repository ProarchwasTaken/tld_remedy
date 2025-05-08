#include <raylib.h>
#include "utils/camera.h"


Camera2D CameraUtils::setupField() {
  Camera2D camera;
  camera.target = {0, 0};
  camera.offset = {213, 120};
  camera.zoom = 1.0;
  camera.rotation = 0;

  return camera;
}
