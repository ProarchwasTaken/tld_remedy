#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/field_sequence.h"
#include "data/session.h"
#include "scenes/field.h"
#include "field/actors/player.h"
#include "field/sequences/incap.h"
#include <plog/Log.h>


IncapSequence::IncapSequence(Session *session) : 
  FieldSequence("Incapacitated Sequence", SequenceID::INCAP)
{
  Companion *companion = &session->companion;
  setupIncapTexture(companion->companion_id);

  PlayerActor::setControllable(false);
  Game::bgm->stop();
}

IncapSequence::~IncapSequence() {
  UnloadTexture(texture);
}

void IncapSequence::setupIncapTexture(CompanionID id) {
  switch (id) {
    case CompanionID::ERWIN: {
      texture = LoadTexture("graphics/cgs/incap_erwin.png");
      break;
    }
  }

  position = {383.0f - texture.width, 207.0f - texture.height};
  tint = WHITE;
  tint.a = 0;
}

void IncapSequence::update() {
  seq_clock += Game::deltaTime() / seq_time;
  seq_clock = Clamp(seq_clock, 0.0, 1.0);

  switch (order) {
    case 0: {
      FieldScene::sfx.play("incapacitated");
      order++;
    }
    case 1: {
      tint.a = Lerp(0, 255, seq_clock);

      if (seq_clock == 1.0) {
        PLOGD << "Texture fade in complete.";
        seq_clock = 0;
        seq_time = 2.0;
        order++;
      }

      break;
    }
    case 2: {
      if (seq_clock == 1.0) {
        PLOGD << "Hiding everything.";
        tint.a = 0;
        seq_clock = 0;
        seq_time = 2.0;
        order++;
      }
      break;
    }
    case 3: {
      if (seq_clock == 1.0) {
        // Temporary
        PLOGD << "Ending sequence.";
        PlayerActor::setControllable(true);
        Game::bgm->play();
        end_sequence = true;
      }
      break;
    }
  }
}

void IncapSequence::draw() {
  ClearBackground(BLACK);
  DrawTextureV(texture, position, tint);
}
