#pragma once
#include <raylib.h>
#include <memory>
#include "base/scene.h"
#include "enums.h"


class Game {
public:
  void init();
  ~Game();
  void setupCanvas();
  void defineColorPalette();

  void start();
  void fadeScreen();

  static float deltaTime();
  static bool debugInfo();
  static void toggleDebugInfo();
  static void setTimeScale(float number);

  static void fadeout(float fade_time);
  static void fadein(float fade_time);

  static constexpr Vector2 CANVAS_RES = {426, 240};
  static constexpr Vector2 WINDOW_RES = {1280, 720};

  static constexpr int target_framerate = 60;

  static Font sm_font;
  static Color* palette;

  #ifndef NDEBUG
  static constexpr bool devmode = true; 
  #else
  static constexpr bool devmode = false;
  #endif // !NDEBUG
private:
  static GameState game_state;
  static bool debug_info;
  static float time_scale;

  static float fade_time;
  static float fade_percentage;
  Color screen_tint = WHITE;

  RenderTexture canvas;
  Rectangle canvas_src;
  Rectangle canvas_dest;

  std::unique_ptr<Scene> scene;
};
