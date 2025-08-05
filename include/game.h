#pragma once
#include <raylib.h>
#include <memory>
#include <random>
#include "base/scene.h"
#include "data/session.h"
#include "enums.h"


class Game {
public:
  void init();
  ~Game();
  void setupCanvas();
  void defineColorPalette();

  void start();
  void topLevelInput();
  void takeScreenshot();
  void toggleFullscreen();

  /* The functionality of this function majorly depends on what state
   * the game is in. Updating the scene, fading the screen, and loading
   * existing sessions; It's all set up so the game could only perform
   * one of these things per frame to reduce complexity.*/
  void gameLogic();
  void fadeScreenProcedure();
  void loadSessionProcedure();
  void initCombatProcedure();
  void endCombatProcedure();
  void sleepProcedure();

  void drawScene();

  static float deltaTime();
  static GameState state() {return game_state;}

  static bool debugInfo();
  static void toggleDebugInfo();
  static void setTimeScale(float number);

  static void saveSession(Session *data);
  static void loadSession();

  static void initCombat(Session *data);
  static void endCombat();

  static void fadeout(float seconds);
  static void fadein(float seconds);

  static void sleep(float seconds);

  static constexpr Vector2 CANVAS_RES = {426, 240};
  static constexpr unsigned int session_version = 2;

  static Font sm_font;
  static Font med_font;
  static Color* palette;
  static std::mt19937_64 RNG;

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

  static float sleep_time;
  float sleep_clock = 0.0;

  static Color flash_color;

  Vector2 window_res = {1280, 720};
  Color screen_tint = WHITE;

  RenderTexture canvas;
  Rectangle canvas_src;
  Rectangle canvas_dest;
  Vector2 canvas_origin;

  std::unique_ptr<Scene> scene;
  static std::unique_ptr<Scene> reserve;
  static std::unique_ptr<Session> loaded_session;
};
