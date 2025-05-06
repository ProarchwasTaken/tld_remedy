#pragma once
#include <raylib.h>
#include <memory>
#include "base/scene.h"


class Game {
public:
  void init();
  void setupCanvas();
  ~Game();

  void start();

  static float deltaTime();
  static bool debugInfo();
  static void toggleDebugInfo();
  static void setTimeScale(float number);

  static constexpr Vector2 CANVAS_RES = {426, 240};
  static constexpr Vector2 WINDOW_RES = {1280, 720};

  static constexpr int target_framerate = 60;

  #ifndef NDEBUG
  static constexpr bool devmode = true; 
  #else
  static constexpr bool devmode = false;
  #endif // !NDEBUG
private:
  std::unique_ptr<Scene> scene;

  RenderTexture canvas;
  Rectangle canvas_src;
  Rectangle canvas_dest;

  static bool debug_info;
  static float time_scale;
};
