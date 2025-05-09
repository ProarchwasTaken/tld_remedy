#include <cassert>
#include <raylib.h>
#include <memory>
#include <plog/Log.h>
#include "scenes/debug_field.h"
#include "game.h"

using std::make_unique;
float Game::time_scale = 1.0;
bool Game::debug_info = false;

void Game::init() {
  InitWindow(WINDOW_RES.x, WINDOW_RES.y, "Project Remedy");
  SetTargetFPS(60);
  setupCanvas();

  scene = make_unique<DebugField>();
  PLOGI << "Time Scale: " << time_scale;
  PLOGI << "Everything should be good to go!";
}

Game::~Game() {
  UnloadRenderTexture(canvas);
  scene.reset();
  PLOGI << "Thanks for playing!";
}

void Game::setupCanvas() {
  PLOGI << "Setting up the canvas...";
  canvas = LoadRenderTexture(CANVAS_RES.x, CANVAS_RES.y);
  canvas_src = {0, 0, CANVAS_RES.x, -CANVAS_RES.y};
  canvas_dest = {0, 0, WINDOW_RES.x, WINDOW_RES.y};
}

void Game::start() {
  assert(scene != nullptr);

  while (!WindowShouldClose()) {
    if (devmode && IsKeyPressed(KEY_F3)) {
      toggleDebugInfo();
    }

    scene->update();

    BeginTextureMode(canvas);
    {
      ClearBackground(BLACK);
      scene->draw();
    }
    EndTextureMode();

    BeginDrawing(); 
    {
      DrawTexturePro(canvas.texture, canvas_src, canvas_dest, {0, 0}, 0, 
                     WHITE);
      if (debug_info) DrawFPS(24, 16);
    }
    EndDrawing();
  }

  CloseWindow();
}

float Game::deltaTime() {
  return (GetFrameTime() * target_framerate) * time_scale;
}

bool Game::debugInfo() {
  return debug_info;
}

void Game::toggleDebugInfo() {
  debug_info = !debug_info;
  PLOGI << "Debug info has been toggled: " << debug_info;
}

void Game::setTimeScale(float new_scale) {
  time_scale = new_scale;
  PLOGI << "Time scale has been changed: " << time_scale;
}
