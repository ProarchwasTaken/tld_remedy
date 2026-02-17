#include <cassert>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <memory>
#include <vector>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "base/panel.h"
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "utils/text.h"
#include "utils/input.h"
#include "utils/menu.h"
#include "utils/math.h"
#include "system/sprite_atlas.h"
#include "menu/panels/dialog.h"
#include "menu/panels/diagnose.h"
#include <plog/Log.h>

using std::string, std::make_unique, std::vector;


DiagnosePanel::DiagnosePanel(Session *session, SpriteAtlas *rest_atlas) {
  id = PanelID::DIAGNOSE;
  frame = LoadTexture("graphics/menu/frames/diagnose.png");
  keybind = &Game::settings.menu_keybinds;

  menu_atlas = &Game::menu_atlas;
  menu_atlas->use();

  assert(rest_atlas != NULL);
  this->rest_atlas = rest_atlas;

  sfx = &Game::menu_sfx;
  sfx->use();

  party = {&session->player, &session->companion};
  current_member = party.begin();
  supplies = &session->supplies;
  updateDisallowed();
  PLOGD << "DiagnosePanel has been initialized.";
}

DiagnosePanel::~DiagnosePanel() {
  menu_atlas->release();
  sfx->release();
}

void DiagnosePanel::updateDisallowed() {
  disallowed.clear();

  Character *party_member = *current_member;
  StatusID status[3];
  std::copy(party_member->status, party_member->status + 3, status);

  for (int x = 0; x < STATUS_LIMIT; x++) {
    StatusID effect = status[x];

    if (effect == StatusID::NONE) {
      DiagnoseOptions option = static_cast<DiagnoseOptions>(x + 1);
      disallowed.emplace(option);
    }
  }

  PLOGD << "Updated disallowed options.";
}

void DiagnosePanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
  }
  else if (panel_mode) {
    panel->update();

    if (panel->terminate) {
      panelTermination();
    }
  }
  else if (!heal_mode) {
    blink_clock += Game::deltaTime();
    menuNavigation();
  }
  else {
    healModeInput();
  }

  portrait.update(percentage);
}

void DiagnosePanel::heightLerp() {
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 192 * percentage;
}

void DiagnosePanel::panelTermination() {
  assert(panel->selected != NULL);
  
  promptHanding();

  panel.reset();
  panel_mode = false;
}

void DiagnosePanel::promptHanding() {
  if (*panel->selected != PromptOptions::YES) {
    return;
  }

  if (*selected == DiagnoseOptions::LIFE) {
    applyHeal();
  }
  else {
    cureEffect();
  }
}

void DiagnosePanel::menuNavigation() {
  if (portrait.fade_clock != 1.0) {
    return;
  }

  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybind->right, gamepad)) {
    MenuUtils::nextOption(party, current_member);
    selected = options.begin();
    updateDisallowed();

    portrait.fade_clock = 0.0;
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybind->left, gamepad)) {
    MenuUtils::prevOption(party, current_member);
    selected = options.begin();
    updateDisallowed();

    portrait.fade_clock = 0.0;
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybind->down, gamepad)) {
    MenuUtils::nextOption(options, selected, &disallowed);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybind->up, gamepad)) {
    MenuUtils::prevOption(options, selected, &disallowed);
    blink_clock = 0.0;
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybind->confirm, gamepad)) {
    selectOption();
    blink_clock = 0.0;
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybind->cancel, gamepad)) {
    state = PanelState::CLOSING;
    sfx->play("menu_cancel");
  }
}

void DiagnosePanel::selectOption() {
  assert(selected != NULL);

  switch (*selected) {
    case DiagnoseOptions::LIFE: {
      enterHealMode();
      break;
    }
    case DiagnoseOptions::EFFECT_1:
    case DiagnoseOptions::EFFECT_2:
    case DiagnoseOptions::EFFECT_3: {
      openEffectDialog();
      break;
    }
  }
}

void DiagnosePanel::enterHealMode() {
  PLOGI << "Entering Heal Mode.";
  Character *party_member = *current_member;

  if (party_member->life == party_member->max_life) {
    PLOGE << "Party Member is already at full Life!";
    sfx->play("menu_cancel");
    return;
  }

  heal_mode = true;
}

void DiagnosePanel::openEffectDialog() {
  PLOGI << "Opening cure ailment dialog.";
  Character *party_member = *current_member;

  int index = static_cast<int>(*selected) - 1;
  assert(index != STATUS_LIMIT);

  PLOGD << "Attempting to access effect at index: " << index;
  StatusID *effect_slot = &party_member->status[index];
  assert(*effect_slot != StatusID::NONE);

  int cost = getSupplyCost(*effect_slot);
  PLOGD << "Supply Cost: " << cost;

  if (*supplies < cost) {
    PLOGE << "Cost is too expensive!";
    sfx->play("menu_cancel");
    return;
  }

  string name = party_member->name;
  string effect_name = getStatusName(*effect_slot);
  PLOGD << "Effect Name: " << effect_name;

  const char *text = TextFormat("Spend %i supplies to fix %s's\n"
                                "%s?", 
                                cost, name.c_str(), effect_name.c_str());

  vector<string> dialog = {text};
  Vector2 position = {16, 183};
  panel = make_unique<DialogPanel>(position, dialog, true);
  panel_mode = true;
}

void DiagnosePanel::healModeInput() {
  bool gamepad = IsGamepadAvailable(0);

  Character *party_member = *current_member;
  float life = party_member->life;
  float max_life = party_member->max_life;

  if (Input::pressed(keybind->right, gamepad)) {
    incHealSegments(life, max_life);
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybind->left, gamepad)) {
    decHealSegments(life, max_life);
    sfx->play("menu_navigate");
  }
  else if (Input::pressed(keybind->confirm, gamepad)) {
    openHealDialog();
    sfx->play("menu_select");
  }
  else if (Input::pressed(keybind->cancel, gamepad)) {
    PLOGI << "Canceling heal mode.";
    heal_mode = false;
    heal_segments = 0;
    sfx->play("menu_cancel");
  }
}

void DiagnosePanel::incHealSegments(float life, float max_life) {
  float life_percentage = life / max_life;
  int life_segments = std::floor(life_percentage * 10);
  if (life_segments + (heal_segments + 1) > 10) {
    sfx->play("menu_cancel");
    return;
  }

  int resulting_supply_cost = (heal_segments + 1) * 2;
  if (*supplies >= resulting_supply_cost) {
    heal_segments++;
    to_be_healed = calculateToBeHealed(life, max_life);
  }
  else {
    sfx->play("menu_cancel");
  }
}

void DiagnosePanel::decHealSegments(float life, float max_life) {
  if (heal_segments != 0) {
    heal_segments--;
    to_be_healed = calculateToBeHealed(life, max_life);
  }
  else {
    sfx->play("menu_cancel");
  }
}

void DiagnosePanel::openHealDialog() {
  PLOGI << "Opening heal dialog.";
  Character *party_member = *current_member;
  string name = party_member->name;

  int cost = heal_segments * 2;
  const char *text = TextFormat("Spend %i supplies to heal %s\n"
                                "for %00.00f Life?",
                                cost, name.c_str(), to_be_healed);

  vector<string> dialog = {text};
  Vector2 position = {16, 183};
  panel = make_unique<DialogPanel>(position, dialog, true);
  panel_mode = true;
}

void DiagnosePanel::applyHeal() {
  assert(heal_mode);
  Character *party_member = *current_member;
  float *life = &party_member->life;
  float max_life = party_member->max_life;
  int cost = heal_segments * 2;

  PLOGI << "Healing " << party_member->name << " for " << to_be_healed <<
    "Life";
  *life = Clamp(*life + to_be_healed, 0, max_life);
  *supplies -= cost;

  heal_mode = false;
  heal_segments = 0;
  sfx->play("menu_item");
}

float DiagnosePanel::calculateToBeHealed(float life, float max_life) {
  float percentage = life / max_life;
  int segments = std::floorf(percentage * 10);
  float heal_seg = heal_segments;

  float to_be_healed = 0;
  float leftover = (percentage * 10) - segments;

  if (leftover != 0) {
    float segment_life = max_life * 0.10;
    to_be_healed = segment_life * (1.0 - leftover);
    heal_seg--;
  }

  percentage = (heal_seg / 10.0);
  to_be_healed += max_life * percentage;
  return to_be_healed;
}

void DiagnosePanel::cureEffect() {
  assert(*selected != DiagnoseOptions::LIFE);
  Character *party_member = *current_member;

  int index = static_cast<int>(*selected) - 1;
  PLOGI << "Curing effect at index: " << index;
  assert(index != STATUS_LIMIT);

  StatusID *effect_slot = &party_member->status[index];
  assert(*effect_slot != StatusID::NONE);

  int cost = getSupplyCost(*effect_slot);
  *supplies -= cost;

  *effect_slot = StatusID::NONE;
  party_member->status_count--;
  assert(party_member->status_count >= 0);

  updateDisallowed();
  MenuUtils::prevOption(options, selected, &disallowed);
  sfx->play("menu_item");
}

void DiagnosePanel::draw() {
  Rectangle source = {0, 0, 274, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);
  drawSupplyCount();

  if (state == PanelState::READY) {
    drawMemberName();
    drawPortrait();
    drawCursor();
    drawLife();
    drawStatus();
  }

  if (panel_mode) {
    panel->draw();
  }
}

void DiagnosePanel::drawSupplyCount() {
  Font *sm_font = &Game::sm_font;
  int sm_size = sm_font->baseSize;
  
  Font *med_font = &Game::med_font;
  int med_size = med_font->baseSize;

  Rectangle *sprite = &rest_atlas->sprites[0];
  Color main_color = Game::palette[51];
  Color count_color = WHITE;

  float offset = 0;
  if (state != PanelState::READY) {
    offset = Math::smoothstep(16, 0, percentage);
    main_color.a = 255 * percentage;
    count_color.a = 255 * percentage;
  }

  Vector2 position = {-offset, 17};
  DrawTextureRec(rest_atlas->sheet, *sprite, position, main_color);

  position = {3 - offset, 9};
  DrawTextEx(*sm_font, "SUPPLIES", position, sm_size, -1, main_color);

  string text = TextFormat("%02i", *supplies);
  position = TextUtils::alignRight(text.c_str(), {96 - offset, 7}, 
                                   *med_font, -2, 0);
  DrawTextEx(*med_font, text.c_str(), position, med_size, -2, 
             count_color);

  if (heal_segments != 0) {
    int cost = heal_segments * 2;
    text = TextFormat("-%i", cost);
    Vector2 position = {96, 7};
    DrawTextEx(*med_font, text.c_str(), position, med_size, -2, 
               Game::palette[33]);
  }
}

void DiagnosePanel::drawMemberName() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Character *party_member = *current_member;
  std::string name = party_member->name;

  Vector2 position = TextUtils::alignCenter(name.c_str(), {250, 25}, 
                                            *font, -2, 0);
  DrawTextEx(*font, name.c_str(), position, txt_size, -2, WHITE);
}

void DiagnosePanel::drawPortrait() {
  Character *party_member = *current_member;
  PartyMemberID id = party_member->member_id;

  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  std::string name = party_member->name;
  for (char &letter : name) {
    letter = std::toupper(letter);
  }

  Vector2 position = TextUtils::alignCenter(name.c_str(), {112, 198}, 
                                            *font, -2, 0);

  DrawTextEx(*font, name.c_str(), position, txt_size, -2, WHITE);
  portrait.draw(id);
}

void DiagnosePanel::drawCursor() {
  int index = static_cast<int>(*selected);
  float y = y_values[index];

  Rectangle *sprite = &menu_atlas->sprites[0];
  Vector2 position = {152, y};
  Color color = WHITE;

  if (!heal_mode) {
    float sin_a = std::sinf(blink_clock * 2.5);
    sin_a = (sin_a / 2) + 0.5;
    color.a = 255 * sin_a;
  }

  DrawTextureRec(menu_atlas->sheet, *sprite, position, color);
}

void DiagnosePanel::drawLife() {
  Character *party_member = *current_member;
  float life = party_member->life;
  float max_life = party_member->max_life;

  Color color;
  if (life < max_life * 0.30) {
    color = Game::palette[33];
  }
  else {
    color = WHITE;
  }

  drawLifeText(life, max_life, color);
  drawGauge(life, max_life, color);
}

void DiagnosePanel::drawLifeText(float life, float max_life, Color color) 
{
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Vector2 position = {163, 40};
  DrawTextEx(*font, "LIFE", position, txt_size, -2, color);

  string text = TextFormat("%02.00f / %02.00f", life, max_life);
  position = TextUtils::alignRight(text.c_str(), {344, 40}, *font, -2, 0);
  DrawTextEx(*font, text.c_str(), position, txt_size, -2, color);

  if (heal_segments != 0) {
    string text = TextFormat("(+%00.02f)", to_be_healed);
    Vector2 position = TextUtils::alignRight(text.c_str(), {294, 40}, 
                                             *font, -2, 0);
    DrawTextEx(*font, text.c_str(), position, txt_size, -2, 
               Game::palette[14]);
  }
}

void DiagnosePanel::drawGauge(float life, float max_life, 
                              Color default_color) 
{
  float percentage = life / max_life;
  int segments = std::floorf(percentage * 10);
  float leftover = 0;

  if (heal_segments == 0) {
    leftover = (percentage * 10) - segments;
  }

  for (int x = 0; x < 10; x++) {
    float offset = 18 * x;
    Vector2 position = {165 + offset, 55};

    Rectangle *sprite;
    Color color;

    if (x < segments) {
      sprite = &menu_atlas->sprites[8];
      color = default_color;
    }
    else if (x < segments + heal_segments) {
      sprite = &menu_atlas->sprites[10];
      color = Game::palette[14]; 
    }
    else if (leftover != 0) {
      sprite = &menu_atlas->sprites[10];
      color = default_color;
      leftover = 0.0;
    }
    else {
      sprite = &menu_atlas->sprites[9];
      color = WHITE;
    }

    DrawTextureRec(menu_atlas->sheet, *sprite, position, color);
  }
}

void DiagnosePanel::drawStatus() {
  Font *font = &Game::med_font;
  int txt_size = font->baseSize;

  Character *party_member = *current_member;
  StatusID status[3];
  std::copy(party_member->status, party_member->status + 3, status);

  for (int x = 0; x < 3; x++) {
    StatusID effect = status[x];
    float offset = 50 * x;
    Vector2 base_position = {163, 66 + offset};

    Color name_color;
    Color desc_color;
    Color cost_color;
    setStatusColors(effect, name_color, desc_color, cost_color);

    string text = getStatusName(effect);
    DrawTextEx(*font, text.c_str(), base_position, txt_size, -2, 
               name_color);

    text = getStatusDesc(effect);
    Vector2 position = Vector2Add(base_position, {0, 16});
    DrawTextEx(*font, text.c_str(), position, txt_size, -2, desc_color);

    int cost = getSupplyCost(effect);
    if (cost > 0) {
      text = TextFormat("%i SUP.", cost);
      position = {344, 66 + offset};
      position = TextUtils::alignRight(text.c_str(), position, *font, -2,
                                       0);
      DrawTextEx(*font, text.c_str(), position, txt_size, -2, cost_color);
    }
  }
}

void DiagnosePanel::setStatusColors(StatusID id, Color &name_color, 
                                    Color &desc_color, Color &cost_color)
{
  if (id == StatusID::NONE) {
    name_color = Game::palette[2];
    desc_color = Game::palette[2];
    cost_color = Game::palette[2];
  }
  else {
    name_color = Game::palette[32];
    desc_color = Game::palette[33];
    cost_color = Game::palette[51];
  }
}

string DiagnosePanel::getStatusName(StatusID id) {
  switch (id) {
    case StatusID::NONE: {
      return "--------";
    }
    case StatusID::BROKEN_ARM: {
      return "Broken Arm";
    }
    case StatusID::CRIPPLED_LEG: {
      return "Crippled Leg";
    }
    case StatusID::MANGLED: {
      return "Mangled Body";
    }
    default: {
      return "INVALID";
    }
  }
}

string DiagnosePanel::getStatusDesc(StatusID id) {
  switch (id) {
    case StatusID::NONE: {
      return "";
    }
    case StatusID::BROKEN_ARM: {
      return 
      "Offence and Intimidation are\n"
      "lowered by 10%";
    }
    case StatusID::CRIPPLED_LEG: {
      return 
      "Speed is decreased by 15%";
    }
    case StatusID::MANGLED: {
      return 
      "Defense and Recover are\n"
      "lowered by 50%";
    }
    default: {
      return "INVALID DESCRIPTION";
    }
  }
}

int DiagnosePanel::getSupplyCost(StatusID id) {
  switch (id) {
    case StatusID::BROKEN_ARM: {
      return 10;
    }
    case StatusID::CRIPPLED_LEG: {
      return 15;
    }
    case StatusID::MANGLED: {
      return 20;
    }
    default: {
      return -1;
    }
  }
}
