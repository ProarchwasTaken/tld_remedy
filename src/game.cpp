#include <raylib.h>
#include <plog/Log.h>
#include "game.h"


int Game::target_framerate = 60;
float Game::time_scale = 1.0;

void Game::init() {
  InitWindow(WINDOW_RES.x, WINDOW_RES.y, "Project Remedy");
  SetTargetFPS(target_framerate);
  setupCanvas();

  PLOGI << "Everything should be good to go!";
}

Game::~Game() {
  UnloadRenderTexture(canvas);
  PLOGI << "Thanks for playing!";
}

void Game::setupCanvas() {
  PLOGI << "Setting up the canvas...";
  canvas = LoadRenderTexture(CANVAS_RES.x, CANVAS_RES.y);
  canvas_src = {0, 0, CANVAS_RES.x, -CANVAS_RES.y};
  canvas_dest = {0, 0, WINDOW_RES.x, WINDOW_RES.y};
}

void Game::start() {
  while (!WindowShouldClose()) {
    BeginTextureMode(canvas);
    {
      ClearBackground(WHITE);
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

