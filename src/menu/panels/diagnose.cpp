#include <cassert>
#include <cstddef>
#include <raylib.h>
#include "base/panel.h"
#include "enums.h"
#include "game.h"
#include "data/session.h"
#include "system/sprite_atlas.h"
#include "menu/panels/diagnose.h"
#include <plog/Log.h>


DiagnosePanel::DiagnosePanel(Session *session, SpriteAtlas *rest_atlas) {
  id = PanelID::DIAGNOSE;
  frame = LoadTexture("graphics/menu/frames/diagnose.png");

  menu_atlas = &Game::menu_atlas;
  menu_atlas->use();

  assert(rest_atlas != NULL);
  this->rest_atlas = rest_atlas;

  sfx = &Game::menu_sfx;
  sfx->use();
  PLOGD << "DiagnosePanel has been initialized.";
}

DiagnosePanel::~DiagnosePanel() {
  menu_atlas->release();
  sfx->release();
}

void DiagnosePanel::update() {
  if (state != PanelState::READY) {
    transitionLogic();
    heightLerp();
  }
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

void DiagnosePanel::draw() {
  Rectangle source = {0, 0, 274, frame_height};
  DrawTextureRec(frame, source, frame_position, WHITE);
}
