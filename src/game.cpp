#include <cassert>
#include <fstream>
#include <ios>
#include <raylib.h>
#include <raymath.h>
#include <memory>
#include <plog/Log.h>
#include <utility>
#include "scenes/field.h"
#include "enums.h"
#include "data/session.h"
#include "game.h"

using std::make_unique, std::ofstream, std::ifstream, std::unique_ptr;

GameState Game::game_state = READY;
unique_ptr<Session> Game::loaded_session;

Font Game::sm_font;
Color *Game::palette;

float Game::fade_percentage = 0.0;
float Game::fade_time = 0.0;

float Game::time_scale = 1.0;
bool Game::debug_info = false;


void Game::init() {
  InitWindow(WINDOW_RES.x, WINDOW_RES.y, 
             "Project Remedy - v" VERSION " " VER_STAGE);
  SetTargetFPS(60);
  setupCanvas();

  sm_font = LoadFont("graphics/fonts/sm_font.png");
  defineColorPalette();

  scene = make_unique<FieldScene>();
  PLOGI << "Time Scale: " << time_scale;
  PLOGI << "Everything should be good to go!";
}

Game::~Game() {
  scene.reset();
  UnloadRenderTexture(canvas);
  UnloadFont(sm_font);
  UnloadImagePalette(palette);
  PLOGI << "Thanks for playing!";
}

void Game::setupCanvas() {
  PLOGI << "Setting up the canvas...";
  canvas = LoadRenderTexture(CANVAS_RES.x, CANVAS_RES.y);
  canvas_src = {0, 0, CANVAS_RES.x, -CANVAS_RES.y};
  canvas_dest = {0, 0, WINDOW_RES.x, WINDOW_RES.y};
}

void Game::defineColorPalette() {
  PLOGI << "Loading the game's color palette...";
  int color_count;
  Image palette = LoadImage("graphics/palette.png");

  this->palette = LoadImagePalette(palette, 56, &color_count);
  PLOGI << "Successfully loaded palette!";
  PLOGI << "Color Count: " << color_count;
  UnloadImage(palette);
}

void Game::start() {
  assert(scene != nullptr);

  while (!WindowShouldClose()) {
    if (devmode && IsKeyPressed(KEY_F3)) {
      toggleDebugInfo();
    }

    gameLogic();
    drawScene();
  }

  CloseWindow();
}

void Game::gameLogic() {
  switch (game_state) {
    case LOADING_SESSION: {
      loadSessionProcedure();
      break;
    }
    case FADING_IN:
    case FADING_OUT: {
      fadeScreenProcedure();
      break;
    }
    case READY: {
      scene->update();
      break;
    }
  }
}

void Game::drawScene() {
  BeginTextureMode(canvas);
  {
    ClearBackground(BLACK);
    scene->draw();
  }
  EndTextureMode();

  BeginDrawing(); 
  {
    DrawTexturePro(canvas.texture, canvas_src, canvas_dest, {0, 0}, 0, 
                   screen_tint);
    if (debug_info) DrawFPS(24, 16);
  }
  EndDrawing();
}

void Game::fadeScreenProcedure() {
  float magnitude = GetFrameTime() / fade_time;
  if (magnitude == 0) {
    return;
  }

  if (game_state == FADING_OUT) {
    fade_percentage -= magnitude;
  }
  else if (game_state == FADING_IN){
    fade_percentage += magnitude;
  }

  fade_percentage = Clamp(fade_percentage, 0.0, 1.0);

  float value = Lerp(0, 255, fade_percentage);
  screen_tint.r = value;
  screen_tint.g = value;
  screen_tint.b = value;

  bool finished_fading = value == 0 || value == 255;
  if (finished_fading) {
    PLOGI << "Screen fade complete.";
    game_state = READY;
  }
}

void Game::loadSessionProcedure() {
  PLOGI << "Initiating load session procedure.";
  scene.reset();
  
  PLOGD << "Loading the field scene with loaded session as argument.";
  assert(loaded_session != nullptr);
  scene = make_unique<FieldScene>(loaded_session.get());

  loaded_session.reset();
  PLOGI << "Procedure complete.";
}

void Game::fadeout(float fade_time) {
  if (game_state != READY && game_state != LOADING_SESSION) {
    return;
  }

  PLOGI << "Fading out the screen.";
  Game::fade_percentage = 1.0;
  Game::fade_time = fade_time;
  game_state = FADING_OUT;
}

void Game::fadein(float fade_time) {
  if (game_state != READY && game_state != LOADING_SESSION) {
    return;
  }

  PLOGI << "Fading in the screen.";
  Game::fade_percentage = 0.0;
  Game::fade_time = fade_time;
  game_state = FADING_IN;
}

void Game::saveSession(Session *data) {
  PLOGI << "Saving the player's current session.";

  ofstream file;
  file.open("data/session.data", std::ios::binary);
  file.write(reinterpret_cast<char*>(data), sizeof(Session));

  file.close();
  PLOGI << "Session data saved successfully.";
}

void Game::loadSession() {
  PLOGI << "Attempting to load session data.";
  ifstream file;
  file.open("data/session.data", std::ios::binary);

  if (!file.is_open()) {
    PLOGE << "'data/session.data' is not found.";
  }

  Session session;
  file.read(reinterpret_cast<char*>(&session), sizeof(Session));

  if (file.fail()) {
    PLOGE << "Error opening file!";
    file.close();
    return;
  }

  file.close();

  PLOGI << "Successfully loaded session data.";
  PLOGD << "Location: " << session.location;
  PLOGD << "Medical Supplies: " << session.supplies;
  PLOGD << "Player Life: " << session.player.life;

  loaded_session = make_unique<Session>(std::move(session));
  game_state = LOADING_SESSION;
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
