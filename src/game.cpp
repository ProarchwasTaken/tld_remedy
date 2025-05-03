#include <cassert>
#include <raylib.h>
#include <memory>
#include <plog/Log.h>
#include "scenes/debug_field.h"
#include "game.h"

using std::make_unique;

int Game::target_framerate = 60;
float Game::time_scale = 1.0;

void Game::init() {
  InitWindow(WINDOW_RES.x, WINDOW_RES.y, "Project Remedy");
  SetTargetFPS(target_framerate);
  setupCanvas();

  scene = make_unique<DebugField>();
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
    scene->update();

    BeginTextureMode(canvas);
    {
      ClearBackground(WHITE);
      scene->draw();
    }
    EndTextureMode();

    BeginDrawing(); 
    {
      DrawTexturePro(canvas.texture, canvas_src, canvas_dest, {0, 0}, 0, 
                     WHITE);
    }
    EndDrawing();
  }

  CloseWindow();
}

