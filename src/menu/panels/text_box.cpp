#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/panel.h"
#include "utils/text.h"
#include "utils/input.h"
#include "menu/panels/text_box.h"
#include <plog/Log.h>

using std::string;


TextBoxPanel::TextBoxPanel(Vector2 position, string name, string type,
                           string text)
{
  id = PanelID::TEXTBOX;
  frame_position = position;
  keybinds = &Game::settings.menu_keybinds;

  sfx = &Game::menu_sfx;

  sm_font = &Game::sm_font;
  sm_size = sm_font->baseSize;

  med_font = &Game::med_font;
  med_size = med_font->baseSize;

  frame_position = position;

  name_text = name;
  name_position = Vector2Add(position, {2, 1});

  type_text = type;
  type_position = Vector2Add(position, {220, 1});
  type_position = TextUtils::alignRight(type_text.c_str(), type_position,
                                        *med_font, -2, 0);

  main_text = text;
  canvas = LoadRenderTexture(219, 133); 
  canvas_position = Vector2Add(position, {2, 16});
  updateCanvas();

  frame = LoadTexture("graphics/menu/frames/textbox.png");
  PLOGD << "Text Box Panel has been initialized.";
}

TextBoxPanel::~TextBoxPanel() {
  UnloadTexture(frame);
  UnloadRenderTexture(canvas);
}

void TextBoxPanel::updateCanvas() {
  Vector2 position = {0, -scroll_y};

  SetTextLineSpacing(12);
  float max_height = canvas.texture.height;
  float height = MeasureTextEx(*sm_font, main_text.c_str(), sm_size, 
                               -3).y;

  if (height > max_height) {
    max_scroll = height - max_height;
  }
  else {
    max_scroll = 0;
  }

  BeginTextureMode(canvas);
  {
    ClearBackground(BLACK);
    DrawTextEx(*sm_font, main_text.c_str(), position, sm_size, -3, WHITE);
  }
  EndTextureMode();
  SetTextLineSpacing(16);
}

void TextBoxPanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
  }
  else {
    scrollingInput();
  }
}

void TextBoxPanel::heightLerp() {
  if (state == PanelState::OPENING) {
    percentage = clock;
  }
  else {
    percentage = 1.0 - clock;
  }

  frame_height = 152 * percentage;
}

void TextBoxPanel::scrollingInput() {
  bool gamepad = IsGamepadAvailable(0);
  if (Input::pressed(keybinds->cancel, gamepad)) {
    sfx->play("menu_cancel");
    state = PanelState::CLOSING;
    return;
  }

  if (max_scroll <= 0) {
    return;
  }

  bool down = Input::down(keybinds->down, gamepad);
  bool up = Input::down(keybinds->up, gamepad);
  int direction = down - up;

  if (direction != 0) {
    scroll_y += (scroll_speed * direction) * Game::deltaTime();
    scroll_y = Clamp(scroll_y, 0.0, max_scroll);
    updateCanvas();
  }
}

void TextBoxPanel::draw() {
  Rectangle source = {0, 0, 224, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);

  if (state == PanelState::READY) {
    DrawTextEx(*med_font, name_text.c_str(), name_position, med_size, -2,
               WHITE);
    DrawTextEx(*med_font, type_text.c_str(), type_position, med_size, -2, 
               Game::palette[2]);
    DrawTextureRec(canvas.texture, canvas_source, canvas_position, WHITE);
  }
}
