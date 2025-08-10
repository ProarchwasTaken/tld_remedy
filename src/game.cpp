#include <string>
#include <cassert>
#include <fstream>
#include <ios>
#include <chrono>
#include <random>
#include <filesystem>
#include <raylib.h>
#include <raymath.h>
#include <memory>
#include <plog/Log.h>
#include "enums.h"
#include "system/sound_atlas.h"
#include "scenes/title.h"
#include "scenes/field.h"
#include "scenes/combat.h"
#include "data/session.h"
#include "data/personal.h"
#include "game.h"

using std::make_unique, std::ofstream, std::ifstream, std::unique_ptr,
std::filesystem::create_directory, std::chrono::system_clock, 
std::string, std::mt19937_64;

GameState Game::game_state = GameState::READY;
unique_ptr<Scene> Game::reserve;

Font Game::sm_font;
Font Game::med_font;

Color *Game::palette;
Color Game::flash_color = {0, 0, 0, 0};
SoundAtlas Game::menu_sfx("menu");
mt19937_64 Game::RNG;

Settings Game::settings;

float Game::fade_percentage = 0.0;
float Game::fade_time = 0.0;

float Game::sleep_time = 0.0;

float Game::time_scale = 1.0;
bool Game::debug_info = false;


void Game::init() {
  loadPersonal();
  InitWindow(window_res.x, window_res.y, 
             "Project Remedy - v" VERSION " " VER_STAGE);
  InitAudioDevice();
  SetMasterVolume(settings.master_volume);
  SetTargetFPS(settings.framerate);

  HideCursor();
  setupCanvas();

  if (settings.fullscreen && PLATFORM == PLATFORM_WINDOWS) {
    toggleFullscreen();
  }

  sm_font = LoadFont("graphics/fonts/sm_font.png");
  med_font = LoadFont("graphics/fonts/med_font.png");
  defineColorPalette();
  
  system_clock::time_point today = system_clock::now();
  long seed = system_clock::to_time_t(today);
  RNG.seed(seed);
  PLOGD << "RNG Seed: " << seed;

  menu_sfx.use();
  scene = make_unique<TitleScene>();
  PLOGI << "Time Scale: " << time_scale;
  PLOGI << "Everything should be good to go!";
}

void Game::loadPersonal() {
  ifstream file;
  file.open("data/personal.data", std::ios::binary);

  if (!file.is_open()) {
    PLOGI << "Existing personal data not found.";
    file.close();
    return;
  }

  Personal data;
  file.read(reinterpret_cast<char*>(&data), sizeof(Personal));

  if (file.fail()) {
    PLOGE << "Something has gone wrong!";
    file.close();
    return;
  }

  file.close();

  if (data.version != personal_version) {
    PLOGE << "Personal data is outdated!";
    return;
  }


  settings = data.settings;
  PLOGD << "Master Volume: " << settings.master_volume;
  PLOGD << "SFX Volume: " << settings.sfx_volume;
  PLOGD << "BGM Volume: " << settings.bgm_volume;
  PLOGD << "Fullscreen: " << settings.fullscreen;
  PLOGD << "Framerate: " << settings.framerate;
  PLOGI << "Loaded the player's personal settings.";
}

Game::~Game() {
  scene.reset();

  if (reserve != nullptr) {
    reserve.reset();
  }

  UnloadRenderTexture(canvas);
  UnloadFont(sm_font);
  UnloadFont(med_font);
  UnloadImagePalette(palette);
  menu_sfx.release();
  assert(menu_sfx.users() == 0);
  
  savePersonal();
  PLOGI << "Thanks for playing!";
}

void Game::savePersonal() {
  Personal data = {personal_version, settings};

  ofstream file;
  file.open("data/personal.data", std::ios::binary);
  file.write(reinterpret_cast<char*>(&data), sizeof(Personal));

  file.close();
  PLOGI << "Saved the player's personal settings and statistics.";
}

void Game::setupCanvas() {
  PLOGI << "Setting up the canvas...";
  if (!IsRenderTextureReady(canvas)) {
    canvas = LoadRenderTexture(CANVAS_RES.x, CANVAS_RES.y);
  }

  canvas_src = {0, 0, CANVAS_RES.x, -CANVAS_RES.y};
  canvas_origin = {window_res.x / 2, window_res.y / 2};
  canvas_dest = {canvas_origin.x, canvas_origin.y, 
    window_res.x, window_res.y};
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
    topLevelInput();
    gameLogic();
    drawScene();
  }

  if (PLATFORM == PLATFORM_WINDOWS) {
    CloseWindow();
    CloseAudioDevice();
  }
}

void Game::topLevelInput() {
  if (devmode && IsKeyPressed(KEY_F3)) {
    toggleDebugInfo();
  }
  if (IsKeyPressed(KEY_F2)) {
    takeScreenshot();
  }
  if (IsKeyPressed(KEY_F11) && PLATFORM == PLATFORM_WINDOWS) {
    toggleFullscreen();
  }
}

void Game::takeScreenshot() {
  if (DirectoryExists("screenshots") == false) {
    PLOGD << "'screenshots' directory not found!";
    create_directory("screenshots");
  }

  system_clock::time_point today = system_clock::now();
  long time = system_clock::to_time_t(today);

  string file_path = "screenshots/remedy_" + 
    std::to_string(time) + ".png";

  PLOGI << "Saved screenshot: '" << file_path << "'";
  Image screenshot = LoadImageFromScreen();
  ExportImage(screenshot, file_path.c_str());
  UnloadImage(screenshot);
}

void Game::toggleFullscreen() {
  PLOGI << "Toggling fullscreen.";
  ToggleBorderlessWindowed();
  window_res.x = GetScreenWidth();
  window_res.y = GetScreenHeight();

  settings.fullscreen = IsWindowState(FLAG_WINDOW_UNDECORATED);
  setupCanvas();
}

void Game::gameLogic() {
  switch (game_state) {
    case GameState::INIT_COMBAT: {
      initCombatProcedure();
      break;
    }
    case GameState::END_COMBAT: {
      endCombatProcedure();
      break;
    }
    case GameState::LOADING_SESSION: {
      loadSessionProcedure();
    }
    case GameState::FADING_IN:
    case GameState::FADING_OUT: {
      fadeScreenProcedure();
      break;
    }
    case GameState::SLEEP: {
      sleepProcedure();
    }
    case GameState::READY: {
      scene->update();
      break;
    }
  }
}

void Game::fadeScreenProcedure() {
  float value = Lerp(0, 255, fade_percentage);
  screen_tint.r = value;
  screen_tint.g = value;
  screen_tint.b = value;

  float magnitude = deltaTime() / fade_time;
  if (magnitude == 0) {
    return;
  }

  if (game_state == GameState::FADING_OUT) {
    fade_percentage -= magnitude;
  }
  else if (game_state == GameState::FADING_IN){
    fade_percentage += magnitude;
  }

  fade_percentage = Clamp(fade_percentage, 0.0, 1.0);

  bool finished_fading = fade_percentage == 0.0 || fade_percentage == 1.0;
  if (finished_fading) {
    PLOGI << "Screen fade complete.";
    game_state = GameState::READY;
  }
}

void Game::sleepProcedure() {
  sleep_clock += deltaTime() / sleep_time;
  if (sleep_clock >= 1.0) {
    PLOGI << "Now returning to normal logic processing.";
    PLOGI << sleep_time;
    game_state = GameState::READY;
    sleep_clock = 0.0;
  }
}

void Game::loadSessionProcedure() {
  PLOGI << "Initiating load session procedure.";
  scene.reset();
   
  assert(reserve != nullptr);
  scene.swap(reserve);

  assert(scene->scene_id == SceneID::FIELD);
  Game::fadein(1.0);
  PLOGI << "Procedure complete.";
}

void Game::initCombatProcedure() {
  static float clock = 0.0;
  static float sequence_time = 1.5;

  clock += deltaTime() / sequence_time;
  clock = Clamp(clock, 0.0, 1.0);

  if (flash_color.a != 0) {
    float percentage = 1.0 - (clock / 0.20);
    percentage = Clamp(percentage, 0.0, 1.0);

    flash_color.a = Lerp(0, 255, percentage);
  }

  if (clock >= 0.20) {
    float unflipped = Clamp(-0.20 + clock, 0.0, 1.0) / 0.10;
    float percentage = Clamp(1.0 - unflipped, 0.0, 1.0);

    canvas_dest.height = Lerp(0, window_res.y, percentage);
    canvas_origin.y = canvas_dest.height / 2;
  }

  if (clock == 1.0) {
    PLOGI << "Switching over to the Combat scene.";
    scene.swap(reserve);
    assert(scene != nullptr && scene->scene_id == SceneID::COMBAT);

    clock = 0.0;
    setupCanvas();
    Game::fadein(0.25);
    fadeScreenProcedure();
  }
}

void Game::endCombatProcedure() {
  PLOGI << "Switching back to the Field scene";
  scene.reset();
  scene.swap(reserve);

  assert(scene != nullptr && scene->scene_id == SceneID::FIELD);
  game_state = GameState::READY;
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
    ClearBackground(BLACK);
    DrawTexturePro(canvas.texture, canvas_src, canvas_dest, 
                   canvas_origin, 0, screen_tint);
    if (flash_color.a != 0) {
      DrawRectangleV({0, 0}, window_res, flash_color);
    }

    if (debug_info) DrawFPS(0, 0);
  }
  EndDrawing();
}

void Game::fadeout(float seconds) {
  switch (game_state) {
    case GameState::READY:
    case GameState::LOADING_SESSION: {
      PLOGI << "Fading out the screen.";
      Game::fade_percentage = 1.0;
      Game::fade_time = seconds;
      game_state = GameState::FADING_OUT;
      break;
    }
    default: {
      PLOGE << "Function cannot be called in this current gamestate!";
      return;
    }
  }
}

void Game::fadein(float seconds) {
  switch (game_state) {
    case GameState::READY:
    case GameState::LOADING_SESSION: 
    case GameState::INIT_COMBAT: {
      PLOGI << "Fading in the screen.";
      Game::fade_percentage = 0.0;
      Game::fade_time = seconds;
      game_state = GameState::FADING_IN;
      break;
    }
    default: {
      PLOGE << "Function cannot be called in this current gamestate!";
      return;
    }
  }
}

void Game::sleep(float seconds) {
  PLOGI << "Pausing game logic for: " << seconds << " seconds";
  Game::sleep_time = seconds;
  game_state = GameState::SLEEP;
}

void Game::newSession(SubWeaponID sub_weapon, CompanionID companion) {
  PLOGI << "Starting a new game.";
  assert(reserve == nullptr);

  reserve = make_unique<FieldScene>(sub_weapon, companion);
  game_state = GameState::LOADING_SESSION;
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
  PLOGD << "Status Count: " << session.player.status_count;

  if (session.version != session_version) {
    PLOGE << "Loaded session data is outdated!";
    return;
  }

  assert(reserve == nullptr);
  reserve = make_unique<FieldScene>(&session);
  game_state = GameState::LOADING_SESSION;
}

bool Game::existingSession() {
  ifstream file;
  file.open("data/session.data", std::ios::binary);

  if (!file.is_open()) {
    PLOGI << "Existing session data not found.";
    return false;
  }

  Session session;
  file.read(reinterpret_cast<char*>(&session), sizeof(Session));

  if (file.fail()) {
    file.close();
    return false;
  }

  file.close();

  if (session.version != session_version) {
    PLOGE << "Loaded session data is outdated!";
    return false;
  }

  PLOGI << "Detected existing session data.";
  return true;
}

void Game::initCombat(Session *data) {
  PLOGI << "Battle Time!";
  assert(reserve == nullptr);

  reserve = make_unique<CombatScene>(data);
  flash_color = WHITE;
  game_state = GameState::INIT_COMBAT;
}

void Game::endCombat() {
  if (game_state == GameState::END_COMBAT) {
    return;
  }

  PLOGI << "Preparing to end combat.";
  assert(reserve != nullptr);

  game_state = GameState::END_COMBAT;
}

float Game::deltaTime() {
  return GetFrameTime() * time_scale;
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
