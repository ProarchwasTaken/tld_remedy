#include <nlohmann/json.hpp>
#include <functional>
#include <string>
#include <sstream>
#include <cstring>
#include <cassert>
#include <fstream>
#include <chrono>
#include <tuple>
#include <random>
#include <filesystem>
#include <raylib.h>
#include <raymath.h>
#include <memory>
#include <ios>
#include "enums.h"
#include "system/noise_effect.h"
#include "system/sprite_atlas.h"
#include "system/sound_atlas.h"
#include "system/music_player.h"
#include "scenes/title.h"
#include "scenes/field.h"
#include "scenes/camp_menu.h"
#include "scenes/rest_menu.h"
#include "scenes/combat.h"
#include "scenes/game_over.h"
#include "data/session.h"
#include "data/personal.h"
#include "utils/math.h"
#include "game.h"
#include <plog/Log.h>

using std::make_unique, std::ofstream, std::ifstream, std::unique_ptr,
std::filesystem::create_directory, std::chrono::system_clock, 
std::string, std::mt19937_64, std::uniform_int_distribution,
nlohmann::json, nlohmann::basic_json, std::tuple;

GameState Game::game_state = GameState::READY;
SessionID Game::current_file = SessionID::UNSAVED;

bool Game::EXIT_GAME = false;
bool Game::SKIP_FRAME = true;
unique_ptr<Scene> Game::reserve;

Font Game::sm_font;
Font Game::med_font;

Color *Game::palette;
Color Game::flash_color = {0, 0, 0, 0};

SpriteAtlas Game::menu_atlas("menu", "menu_elements");
SoundAtlas Game::menu_sfx("menu");
mt19937_64 Game::RNG;

unique_ptr<MusicPlayer> Game::bgm;
unique_ptr<NoiseEffect> Game::noise;

Settings Game::settings;
HashKeys Game::hash_keys;

Color Game::screen_tint = WHITE;
float Game::fade_clock = 0.0;
float Game::fade_time = 0.0;

float Game::sleep_time = 0.0;

bool Game::debug_info = false;
float Game::time_scale = 1.0;

double Game::session_playtime = 0.0;
bool Game::run_timer = false;


Game::Game(int argc, char *argv[]) {
  PLOGI << "Evaluating command line arguments.";

  for (int x = 0; x < argc; x++) {
    string arg = argv[x];

    if (arg == "-m" && x + 1 != argc) {
      quick_load = true;
      ql_map = argv[x + 1];
    }
    else if (arg == "--bat") {
      ql_weapon = SubWeaponID::BAT;
      quick_load = true;
    }
    else if (arg == "--xander") {
      ql_companion = CompanionID::XANDER;
      quick_load = true;
    }
  }
}

void Game::init() {
  loadPersonal();
  InitWindow(window_res.x, window_res.y, 
             "Project Remedy - v" VERSION " " VER_STAGE);
  InitAudioDevice();
  SetMasterVolume(settings.master_volume);
  SetTargetFPS(settings.framerate);
  SetTextLineSpacing(16);

  Image icon = LoadImage("graphics/icon.png");
  SetWindowIcon(icon);
  UnloadImage(icon);

  setupCanvas();

  if (settings.fullscreen) {
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
  bgm = make_unique<MusicPlayer>();
  noise = make_unique<NoiseEffect>();

  if (!quick_load) {
    scene = make_unique<TitleScene>();
  }
  else {
    PLOGI << "Quick loading map: " << ql_map;
    scene = make_unique<FieldScene>(ql_map, ql_weapon, ql_companion);
    run_timer = true;
  }

  PLOGI << "Time Scale: " << time_scale;
  PLOGI << "Everything should be good to go!";
}

Game::~Game() {
  bgm.reset();
  noise.reset();
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
  hash_keys = data.hash_keys;
  PLOGI << "Loaded the player's personal settings.";
}

void Game::savePersonal() {
  Personal data = {personal_version, hash_keys, settings};

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
  while (!EXIT_GAME && !WindowShouldClose()) {
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
  if (IsKeyPressed(KEY_F11)) {
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
  settings.fullscreen = IsWindowState(FLAG_WINDOW_UNDECORATED);

  if (settings.fullscreen) {
    int monitor = GetCurrentMonitor();
    window_res.x = GetMonitorWidth(monitor);
    window_res.y = GetMonitorHeight(monitor);
  }
  else {
    window_res = {1278, 720};
  }
 
  setupCanvas();
}

void Game::newGame(SubWeaponID sub_weapon, CompanionID companion) {
  PLOGI << "Starting a new game.";
  assert(reserve == nullptr);

  bgm->stop();
  reserve = make_unique<FieldScene>(sub_weapon, companion);

  session_playtime = 0;
  current_file = UNSAVED;
  run_timer = true;

  game_state = GameState::SWITCHING_SCENE;
}

void Game::saveGame(Session *data, SessionID file_id) {
  assert(file_id != SessionID::UNSAVED);
  PLOGI << "Saving the player's current session on File " << file_id;

  current_file = file_id;
  data->file_id = file_id;
  data->playtime = session_playtime;

  string path;
  if (file_id != TEMPORARY) {
    path = "data/session" + std::to_string(file_id) + ".data";
  }
  else {
    path = "data/.temp_session"; 
  }

  ofstream file;
  file.open(path, std::ios::binary);

  file.write(reinterpret_cast<char*>(data), sizeof(Session));
  file.close();
  PLOGD << "Finished writing to file."; 
  
  string result = getSessionKey(data);
  PLOGD << "Session Key: '" << result << "'";

  switch (file_id) {
    case FILE1: {
      std::strcpy(hash_keys.file1, result.c_str());
      break;
    }
    case FILE2: {
      std::strcpy(hash_keys.file2, result.c_str());
      break;
    }
    case FILE3: {
      std::strcpy(hash_keys.file3, result.c_str());
      break;
    }
    case TEMPORARY: {
      std::strcpy(hash_keys.temp, result.c_str());
      break;
    }
    default: {
    }
  }

  PLOGI << "Game has been saved.";
}

bool Game::loadGame(SessionID file_id) {
  PLOGI << "Attempting to load session " << file_id << " from external " 
    << "file.";

  Session session;
  try {
    session = validateSession(file_id);
    PLOGI << "Session data has been successfully retrieved.";
  } 
  catch (SessionException error_code) {
    PLOGE << "Attempt to load session has failed!";
    PLOGE << "Error Code: " << error_code;
    return false;
  }

  assert(reserve == nullptr);
  bgm->stop();
  reserve = make_unique<FieldScene>(&session);

  session_playtime = session.playtime;
  current_file = session.file_id;
  run_timer = true;

  game_state = GameState::SWITCHING_SCENE;
  return true;
}

void Game::exitGame() {
  PLOGI << "Exit function has been called!";
  EXIT_GAME = true;
}

Session Game::validateSession(SessionID file_id) {
  assert(file_id != SessionID::UNSAVED);

  string path;
  if (file_id != TEMPORARY) {
    path = "data/session" + std::to_string(file_id) + ".data";
  }
  else {
    path = "data/.temp_session"; 
  }

  ifstream file;
  file.open(path, std::ios::binary);

  if (!file.is_open()) {
    PLOGE << "Existing session data not found!";
    throw SESSION_NOT_FOUND;
  }

  Session session;
  file.read(reinterpret_cast<char*>(&session), sizeof(Session));

  if (file.fail()) {
    PLOGE << "Reading operation has failed!";
    file.close();
    throw SESSION_FAILURE;
  }

  file.close();

  if (session.version != session_version) {
    PLOGE << "Loaded session data is outdated!";
    throw SESSION_OUTDATED;
  }

  if (session.file_id != file_id) {
    PLOGE << "File ID does not match!";
    throw SESSION_MISMATCHED_ID;
  }

  string session_key = getSessionKey(&session);
  PLOGD << "Hash key of loaded session: '" << session_key << "'";

  string expected_key;
  switch (file_id) {
    case FILE1: {
      expected_key = hash_keys.file1;
      break;
    }
    case FILE2: {
      expected_key = hash_keys.file2;
      break;
    }
    case FILE3: {
      expected_key = hash_keys.file3;
      break;
    }
    case TEMPORARY: {
      expected_key = hash_keys.temp;
      break;
    }
    default: {
    }
  }

  PLOGD << "Expected Session Key: '" << expected_key << "'";
  if (session_key != expected_key) {
    PLOGE << "Session key does not match what was expected!";
    PLOGE << "The loaded session has might've been corrupted or modified" 
      << "externally.";
    throw SESSION_CORRUPTED;
  }

  PLOGI << "Session [ID: " << file_id << "] has been validated.";
  return session;
}

string Game::getSessionKey(Session *data) {
  char *ptr = reinterpret_cast<char*>(data);
  int size = sizeof(Session);

  std::hash<string> session_hash;
  string raw(ptr, size);

  std::stringstream stream;
  stream << std::hex << session_hash(raw);

  string result = stream.str();
  assert(result.size() <= SAVE_KEY_SIZE);
  return result;
}

void Game::gameLogic() {
  if (SKIP_FRAME) {
    PLOGD << "Skipping frame.";
    SKIP_FRAME = false;
    return;
  }

  if (run_timer) {
    session_playtime += GetFrameTime();
  }

  switch (game_state) {
    case GameState::TOGGLE_FULLSCREEN: {
      toggleFullscreen();
      game_state = GameState::READY;
      break;
    }
    case GameState::OPEN_CAMPMENU: {
      openCampMenuProcedure();
      break;
    }
    case GameState::OPEN_RESTMENU: {
      openRestMenuProcedure();
      break;
    }
    case GameState::INIT_COMBAT: {
      initCombatProcedure();
      break;
    }
    case GameState::DEATH_SAVE: {
      deathsaveProcedure();
      scene->update();
      break;
    }
    case GameState::GAME_OVER: {
      gameoverProcedure();
      scene->update();
      break;
    }
    case GameState::RETURN_TO_FIELD: {
      returnFieldProcedure();
      break;
    }
    case GameState::SWITCHING_SCENE: {
      switchSceneProcedure();
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

  bgm->update();
  noise->update();
}

void Game::fadeScreenProcedure() {
  float value = Lerp(0, 255, fade_clock);
  screen_tint.r = value;
  screen_tint.g = value;
  screen_tint.b = value;

  float magnitude = deltaTime() / fade_time;
  if (magnitude == 0) {
    return;
  }

  if (game_state == GameState::FADING_OUT) {
    fade_clock -= magnitude;
  }
  else if (game_state == GameState::FADING_IN){
    fade_clock += magnitude;
  }

  fade_clock = Clamp(fade_clock, 0.0, 1.0);

  bool finished_fading = fade_clock == 0.0 || fade_clock == 1.0;
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

void Game::switchSceneProcedure() {
  PLOGI << "Proceeding to switch to loaded scene.";
  scene.reset();
   
  assert(reserve != nullptr);
  scene.swap(reserve);

  Game::fadein(1.0);
  SKIP_FRAME = true;
  PLOGI << "Procedure complete.";
}

void Game::openCampMenuProcedure() {
  static float clock = 0.0;
  static float sequence_time = 1.0;

  clock += deltaTime() / sequence_time;
  clock = Clamp(clock, 0.0, 1.0);

  float percentage = Clamp(clock / 0.30, 0.0, 1.0);
  flash_color.a = Lerp(0, 255, percentage);

  float end_height = window_res.y * 0.008;
  canvas_dest.height = Lerp(window_res.y, end_height, percentage);
  canvas_origin.y = canvas_dest.height / 2;

  percentage = Clamp((-0.30 + clock) / 0.20, 0.0, 1.0);
  canvas_dest.width = Math::smoothstep(window_res.x, 0, percentage);
  canvas_origin.x = canvas_dest.width / 2;

  if (clock == 1.0) {
    PLOGI << "Switching over to the Camp Menu scene.";
    scene.swap(reserve);
    assert(scene != nullptr && scene->scene_id == SceneID::CAMP_MENU);
    setupCanvas();

    clock = 0.0;
    flash_color.a = 0;

    game_state = GameState::READY;
    menu_sfx.play("menu_camp");
    return;
  }
}

void Game::openRestMenuProcedure() {
  PLOGI << "Switching over to the Rest Menu scene.";
  scene.swap(reserve);
  assert(scene != nullptr && scene->scene_id == SceneID::REST_MENU);

  game_state = GameState::READY;
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
    noise->setAlpha(0.0);
    setupCanvas();

    bgm->play();
    bgm->setBaseVolume(0.0);
    bgm->fade(1.0, 0.25);

    Game::fadein(0.25);
    fadeScreenProcedure();
  }
}

void Game::deathsaveProcedure() {
  static float clock = 0.0;
  static float sequence_time = 4.0;

  clock += GetFrameTime() / sequence_time;
  clock = Clamp(clock, 0.0, 1.0);

  float percentage = Clamp(clock / 0.125, 0.0, 1.0);
  flash_color.a = Lerp(0, 255, percentage);

  if (clock == 1.0) {
    PLOGI << "Sequence complete.";
    clock = 0.0;

    PLOGI << "Moving on to the endCombatProcedure.";
    assert(scene->scene_id == SceneID::COMBAT);
    CombatScene *combat = static_cast<CombatScene*>(scene.get());
    combat->endCombatProcedure();

    noise->setAlpha(0.0);
    noise->setTint(WHITE);
  }
}

void Game::gameoverProcedure() {
  static float clock = 0.0;
  static float sequence_time = 3.42;
  
  clock += GetFrameTime() / sequence_time;
  clock = Clamp(clock, 0.0, 1.0);

  float percentage = Clamp(clock / 0.10, 0.0, 1.0);
  flash_color.a = Lerp(255, 0, percentage);

  percentage = Clamp((-0.305 + clock) / 0.10, 0.0, 1.0);
  float end_height = window_res.y * 0.004;
  canvas_dest.height = Lerp(window_res.y, end_height, percentage);
  canvas_origin.y = canvas_dest.height / 2;

  percentage = Clamp(clock / 0.405, 0.0, 1.0);
  noise->setAlpha(percentage);

  if (clock == 1.0) {
    PLOGI << "Switching over to the Game Over scene.";
    scene.swap(reserve);

    assert(scene != nullptr && scene->scene_id == SceneID::GAME_OVER);
    setupCanvas();

    reserve.reset();
    flash_color.a = 0;
    clock = 0.0;

    game_state = GameState::READY;
    setTimeScale(1.0);

    noise->setAlpha(0.0);
    noise->setTint(WHITE);
  }
}

void Game::returnFieldProcedure() {
  PLOGI << "Switching back to the Field scene";
  SceneID from = scene->scene_id;
  scene.reset();
  scene.swap(reserve);

  assert(scene != nullptr && scene->scene_id == SceneID::FIELD);
  FieldScene *field = static_cast<FieldScene*>(scene.get());
  field->onSceneReturn(from);
  flash_color.a = 0;

  Game::fadein(0.5);
  SKIP_FRAME = true;
}

void Game::drawScene() {
  BeginTextureMode(canvas);
  {
    ClearBackground(BLACK);
    scene->draw();
    noise->draw();
  }
  EndTextureMode();

  BeginDrawing(); 
  {
    ClearBackground(BLACK);
    DrawTexturePro(canvas.texture, canvas_src, canvas_dest, 
                   canvas_origin, 0, screen_tint);

    if (flash_color.a != 0) {
      DrawRectanglePro(canvas_dest, canvas_origin, 0, flash_color);
    }

    if (debug_info) DrawFPS(0, 0);
  }
  EndDrawing();
}

float Game::deltaTime() {
  return GetFrameTime() * time_scale;
}

void Game::setTimeScale(float new_scale) {
  time_scale = new_scale;
  PLOGI << "Time scale has been changed: " << time_scale;
}

void Game::fullscreenCheck() {
  if (settings.fullscreen != IsWindowState(FLAG_WINDOW_UNDECORATED)) {
    game_state = GameState::TOGGLE_FULLSCREEN;
  }
}

void Game::loadTitleScreen() {
  PLOGI << "Returning to Title Scene";
  if (reserve != nullptr) {
    reserve.reset();
  }

  reserve = make_unique<TitleScene>();
  game_state = GameState::SWITCHING_SCENE;

  session_playtime = 0.0;
  run_timer = false;
}

void Game::openCampMenu(Session *data, CampMenuOption *shortcut) {
  assert(reserve == nullptr);

  reserve = make_unique<CampMenuScene>(data, shortcut);
  flash_color = WHITE;
  flash_color.a = 0;

  game_state = GameState::OPEN_CAMPMENU;
  bgm->fade(0.25, 1.8);

  menu_sfx.play("menu_camp_open");
  SKIP_FRAME = true;
}

void Game::openRestMenu(Session *data) {
  assert(reserve == nullptr);

  bgm->stop();
  reserve = make_unique<RestMenuScene>(data);

  game_state = GameState::OPEN_RESTMENU;
  SKIP_FRAME = true;
}

void Game::initCombat(Session *data, TroopID id, int reward) {
  PLOGI << "Battle Time! (Forced Style!)";
  assert(reserve == nullptr);

  reserve = make_unique<CombatScene>(data, id, reward, true);
  flash_color = WHITE;

  noise->setTint(WHITE);
  noise->setAlpha(0.10);

  game_state = GameState::INIT_COMBAT;
  SKIP_FRAME = true;
}

void Game::initCombat(Session *session, TroopID id) {
  PLOGI << "Battle Time!";
  assert(reserve == nullptr);
  bgm->stop();

  ifstream file("data/troops.json");
  json parsed_data = json::parse(file);
 
  string location = session->location;
  basic_json pool = parsed_data.at(location);

  int reward;
  if (id == TroopID::INVALID) {
    PLOGI << "Selecting a random troop in pool: " << location; 
    tuple<TroopID, int> troop = selectRandomTroop(pool);
    std::tie(id, reward) = troop;
  }
  else {
    PLOGI << "Getting reward assigned for Troop ID: " << 
      static_cast<int>(id);
    reward = getTroopReward(id, pool);
  }

  file.close();

  reserve = make_unique<CombatScene>(session, id, reward, false);
  flash_color = WHITE;

  noise->setTint(WHITE);
  noise->setAlpha(0.10);

  game_state = GameState::INIT_COMBAT;
  SKIP_FRAME = true;
}

tuple<TroopID, int> Game::selectRandomTroop(json &pool) {
  int sum_of_weight = 0;
  for (basic_json troop : pool) {
    int id = troop.at("id");
    int weight = troop.at("weight");
    PLOGD << "{Troop ID: " << id << ", Weight: " << weight << "}";

    sum_of_weight += weight;
  }

  PLOGD << "Weight Sum: " << sum_of_weight;
  
  uniform_int_distribution<int> range(1, sum_of_weight);
  int random_num = range(RNG);
  PLOGD << "RNG Value: " << random_num;

  TroopID troop_id = TroopID::INVALID;
  int reward;
  bool successful = false;
  for (basic_json troop : pool) {
    int weight = troop.at("weight");

    if (random_num <= weight) {
      int id = troop.at("id");
      troop_id = static_cast<TroopID>(id);

      reward = troop.at("reward");

      PLOGI << "Troop Selected: {ID: " << id << ", Reward: " << reward 
        << "}";
      successful = true;
      break;
    }
    else {
      random_num -= weight;
    }
  }

  assert(successful);
  return {troop_id, reward};
}

int Game::getTroopReward(TroopID troop_id, nlohmann::json &pool) {
  int id = static_cast<int>(troop_id);
  for (basic_json troop : pool) {
    if (troop.at("id") == id) {
      int reward = troop.at("reward");
      PLOGI << "Reward Found: " << reward;
      return reward;
    }
  }

  PLOGE << "Failed to find reward associated with Troop ID: " << id;
  return 0;
}

void Game::returnToField() {
  if (game_state == GameState::RETURN_TO_FIELD) {
    return;
  }

  PLOGI << "Preparing to return to the Field scene..";
  assert(reserve != nullptr);
  assert(reserve->scene_id == SceneID::FIELD);

  game_state = GameState::RETURN_TO_FIELD;
}

void Game::deathsave() {
  PLOGI << "YOU LOSE! But there's still hope...";

  flash_color = BLACK;
  flash_color.a = 0.0;
  bgm->fade(0.0, 1.0);

  game_state = GameState::DEATH_SAVE;
}

void Game::gameover(string reason) {
  PLOGI << "GAME OVER! Reason: " << reason;
  if (reserve != nullptr) {
    reserve.reset();
  }

  reserve = make_unique<GameOverScene>(reason);
  flash_color = palette[32];
  flash_color.a = 255;

  noise->setTint(palette[32]);

  setTimeScale(0.75);
  bgm->stop();
  menu_sfx.play("gameover");

  game_state = GameState::GAME_OVER;
  SKIP_FRAME = true;
  run_timer = false;
}

void Game::fadeout(float seconds) {
  switch (game_state) {
    case GameState::READY:
    case GameState::SWITCHING_SCENE: {
      PLOGI << "Fading out the screen.";
      Game::screen_tint = WHITE;
      Game::fade_clock = 1.0;
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
    case GameState::RETURN_TO_FIELD:
    case GameState::SWITCHING_SCENE: 
    case GameState::INIT_COMBAT: {
      PLOGI << "Fading in the screen.";
      Game::screen_tint = BLACK;
      Game::fade_clock = 0.0;
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
  if (game_state == GameState::GAME_OVER) {
    return;
  }

  if (game_state == GameState::DEATH_SAVE) {
    return;
  }

  PLOGI << "Pausing game logic for: " << seconds << " seconds";
  Game::sleep_time = seconds;
  game_state = GameState::SLEEP;
}
