#include <cassert>
#include <cstddef>
#include <vector>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/actor.h"
#include "base/field_sequence.h"
#include "data/field_event.h"
#include "field/actors/player.h"
#include "field/actors/companion.h"
#include "field/system/field_handler.h"
#include "field/sequences/ds_erwin_severe.h"
#include <plog/Log.h>

using std::string, std::vector;


DSErwinSevere::DSErwinSevere() : 
  FieldSequence("DS Erwin Severe", SequenceID::DS_ERWIN_SEVERE)
{
  Actor *ptr = Actor::getActor(ActorType::PLAYER);
  player = static_cast<PlayerActor*>(ptr);

  ptr = Actor::getActor(ActorType::COMPANION);
  companion = static_cast<CompanionActor*>(ptr);

  setup();
}

void DSErwinSevere::setup() {
  assert(player != NULL);
  assert(companion != NULL);

  player->sprite = &player->atlas.sprites[12];
  player->direction = Direction::DOWN;
  player->lock_sprite = true;

  companion->sprite = &companion->atlas.sprites[12];
  companion->position = Vector2Add(player->position, {24, 0});
  companion->direction = Direction::LEFT;
  companion->lock_sprite = true;

  companion->move_points.clear();
  companion->follow_player = false;

  companion->rectExCorrection(companion->collis_box, 
                              companion->bounding_box);
  PLOGI << "Sequence has been setup.";
}

void DSErwinSevere::update() {
  vector<string> dialog;
  seq_clock += Game::deltaTime() / seq_time;
  seq_clock = Clamp(seq_clock, 0.0, 1.0);

  switch (order) {
    case 0: {
      dialog = {
        "C'mon...! C'mon...!",
        "Mary, please wake up!"
      };
      FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG, dialog,
                                           false, false, true, true, 
                                           1.0f);

      seq_clock = 0.0;
      order++;
      break;
    }
    case 1: {
      if (seq_clock == 1.0) {
        Game::fadein(1.5);
        black_out = false;

        seq_clock = 0.0;
        order++;
      }

      break;
    }
    case 2: {
      if (seq_clock == 1.0) {
        companion->emote = &companion->emotes.sprites[0];

        dialog = {"Egh... E-Erwin?"};
        FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX,
                                               "Mary", "Martyr", dialog,
                                               false, true, false, true);
        order++;
      }

      break;
    }
    case 3: {
      player->sprite = &player->atlas.sprites[13];
      companion->emote = &companion->emotes.sprites[6];

      dialog = {"Ah, thank goodness you're still alive!"};
      FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX,
                                             "Erwin", "Lost Vagabond",
                                             dialog, false, true, true,
                                             true);
      order++;
      break;
    }
    case 4: {
      player->emote = &player->emotes.sprites[9];
      companion->emote = NULL;

      dialog = {
        "I... feel like utter trash...",
        "H-How... bad is it...?"
      };
      FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX,
                                             "Mary", "Martyr", dialog,
                                             false, true, true, false);
      order++;
      break;
    }
    case 5: {
      player->emote = NULL;
      companion->emote = &companion->emotes.sprites[2];

      seq_time = 1.0;
      seq_clock = 0.0;
      order++;
      break;
    }
    case 6: {
      if (seq_clock == 1.0) {
        companion->lock_sprite = false;
        seq_time = 0.5;
        seq_clock = 0.0;
        order++;
      }

      break;
    }
    case 7: {
      if (seq_clock == 1.0) {
        companion->emote = NULL;
        companion->direction = Direction::DOWN;
        player->sprite = &player->atlas.sprites[14];

        dialog = {"I-I see..."};
        FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX,
                                               "Mary", "Martyr", dialog,
                                               false, true, false, false);
        order++;
      }

      break;
    }
    case 8: {
      companion->direction = Direction::LEFT;
      dialog = {
        "Think you can still patch yourself\n"
        "up?",
        "Actually, you don't have to answer\n"
        "that. I'll help you."
      };

      FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX,
                                             "Erwin", "Lost Vagabond",
                                             dialog, false, true, false,
                                             false);
      order++;
      break;
    }
    case 9: {
      Game::fadeout(1.0);
      order++;
      break;
    }
    case 10: {
      PLOGD << "Ending Sequence.";
      PlayerActor::setControllable(true);
      player->sprite = &player->atlas.sprites[1];
      player->lock_sprite = false;

      companion->sprite = &companion->atlas.sprites[10];
      companion->lock_sprite = false;
      companion->follow_player = true;

      Game::bgm->play();
      Game::fadein(0.5);
      end_sequence = true;
      break;
    }
  }
}

void DSErwinSevere::draw() {
  if (black_out) {
    ClearBackground(BLACK);
  }
}
