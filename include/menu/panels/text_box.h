#pragma once
#include <string>
#include <raylib.h>
#include "base/panel.h"
#include "data/keybinds.h"
#include "system/sound_atlas.h"


class TextBoxPanel : public Panel {
public:
  TextBoxPanel(Vector2 position, std::string name, std::string type, 
               std::string text);
  ~TextBoxPanel();

  void updateCanvas();

  void update() override;
  void heightLerp();
  void scrollingInput();

  void draw() override;
private:
  SoundAtlas *sfx;
  MenuKeybinds *keybinds;
  Texture frame;

  Vector2 frame_position;
  float frame_height = 0;
  float percentage = 0.0;

  Font *sm_font;
  int sm_size;

  Font *med_font;
  int med_size;

  std::string name_text;
  Vector2 name_position;

  std::string type_text;
  Vector2 type_position;

  std::string main_text;

  RenderTexture canvas;
  Vector2 canvas_position;
  Rectangle canvas_source = {0, 0, 219, -133};

  float scroll_speed = 96;

  float scroll_y = 0;
  float max_scroll = 0;
};
