#pragma once
#include <raylib.h>


class Game {
public:
  void init();
  ~Game();
  void setupCanvas();

  void start();

  static constexpr Vector2 CANVAS_RES = {426, 240};
  static constexpr Vector2 WINDOW_RES = {1280, 720};
  static int target_framerate;
  static float time_scale;
private:
  RenderTexture canvas;
  Rectangle canvas_src;
  Rectangle canvas_dest;
};
