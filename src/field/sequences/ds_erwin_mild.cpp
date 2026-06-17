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
#include "field/sequences/ds_erwin_mild.h"
#include <plog/Log.h>

using std::string, std::vector;


DSErwinMild::DSErwinMild() : 
  FieldSequence("Death Save - Erwin Sequence", SequenceID::DS_ERWIN_MILD)
{
  Actor *ptr = Actor::getActor(ActorType::PLAYER);
  player = static_cast<PlayerActor*>(ptr);

  ptr = Actor::getActor(ActorType::COMPANION);
  companion = static_cast<CompanionActor*>(ptr);

  setup();
}

void DSErwinMild::setup() {
  assert(player != NULL);
  assert(companion != NULL);

  player->sprite = &player->atlas.sprites[13];
  player->direction = Direction::DOWN;
  player->lock_sprite = true;

  companion->position = Vector2Add(player->position, {24, 0});
  companion->direction = Direction::LEFT;

  companion->move_points.clear();
  companion->follow_player = false;

  companion->rectExCorrection(companion->collis_box, 
                              companion->bounding_box);
  PLOGI << "Sequence has been setup.";
}

void DSErwinMild::update() {
  vector<string> dialog;
  seq_clock += Game::deltaTime() / seq_time;
  seq_clock = Clamp(seq_clock, 0.0, 1.0);

  switch (order) {
    case 0: {
      dialog = {"r...y...!"};
      FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG, dialog, 
                                           false, false, true, true, 
                                           0.5f);
      seq_clock = 0.0;
      order++;
      break;
    }  
    case 1: {
      if (seq_clock == 1.0) {
        dialog = {"M..a..r..y..!"};
        FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG, 
                                             dialog, false, false, true, 
                                             true, 0.5f);
        seq_clock = 0.0;
        order++;
      }

      break;
    }
    case 2: {
      if (seq_clock == 1.0) {
        dialog = {"Uuuuugggghhh...."};
        FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX, 
                                               "Mary", "Martyr", dialog, 
                                               false, false, true, true, 
                                               0.25f);
        seq_clock = 0.0;
        order++;
      }

      break;
    }
    case 3: {
      if (seq_clock == 1.0) {
        dialog = {"MARY!!!"};
        FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX, 
                                               "Erwin", "Lost Vagabond", 
                                               dialog, false, true, true,
                                               true, 0.50f);
        black_out = false;
        order++;
      }

      break;
    }
    case 4: {
      dialog = {"Huh-"};
      FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX, 
                                             "Mary", "Martyr", dialog,
                                             false, true, true, true,
                                             0.25f);
      player->sprite = &player->atlas.sprites[4];
      player->emote = &Actor::emotes.sprites[1];
      order++;
      break;
    }
    case 5: {
      dialog = {"Tssss....."};
      FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX, 
                                             "Mary", "Martyr", dialog,
                                             false, true, true, false);
      player->sprite = &player->atlas.sprites[14];
      player->emote = NULL;

      companion->emote = &Actor::emotes.sprites[0];
      order++;
      break;
    }
    case 6: {
      Vector2 next_position = Vector2Add(companion->position, {-8, 0});
      companion->emote = NULL;

      MovePoint point = {next_position, Direction::LEFT}; 
      companion->move_points.push_front(point);
      order++;
      break;
    }
    case 7: {
      if (companion->move_points.empty()) {
        companion->lock_sprite = true;
        companion->sprite = &companion->atlas.sprites[12];

        dialog = {"Don't move. You'll only make it worse."};

        FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX,
                                               "Erwin", "Lost Vagabond",
                                               dialog, false, true, false,
                                               true);
        order++;
      }

      break;
    }
    case 8: {
      dialog = {"What.. What happened?"};
      FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX, 
                                             "Mary", "Martyr", dialog,
                                             false, true, true, false);
      order++;
      break;
    }
    case 9: {
      companion->emote = &companion->emotes.sprites[2];
      companion->lock_sprite = false;
      seq_time = 1.0;
      seq_clock = 0.0;
      order++;
      break;
    }
    case 10: {
      if (seq_clock == 1.0) {
        companion->emote = NULL;
        companion->direction = Direction::UP;

        dialog = {
          "The situation got out of hand,\n"
          "and I had to pull your body out\n"
          "of that trainwreck.",
          "Ended up taking a couple hits just\n"
          "to get us to safety."
        };
        FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX,
                                               "Erwin", "Lost Vagabond",
                                               dialog, false, true, false,
                                               false);

        order++;
      }

      break;
    }
    case 11: {
      player->emote = &player->emotes.sprites[9];
      seq_time = 0.50;
      seq_clock = 0.0;
      order++;
      break;
    }
    case 12: {
      if (seq_clock == 1.0) {
        player->sprite = &player->atlas.sprites[13];
        player->emote = NULL;

        dialog = {"Sorry..."};
        FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX, 
                                               "Mary", "Martyr", dialog,
                                               false, true, true, false);
        order++;
      }

      break;
    }
    case 13: {
      companion->direction = Direction::LEFT;

      dialog = {
        "No... no, it's alright.",
        "We're both human afterall..."
      };

      FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX,
                                             "Erwin", "Lost Vagabond",
                                             dialog, false, true, true,
                                             true);
      order++;
      break;
    }
    case 14: {
      companion->direction = Direction::DOWN;
      dialog = {
        "We have to accept that these things\n"
        "happen, and do our best to prevent it."
      };

      FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX,
                                             "Erwin", "Lost Vagabond",
                                             dialog, false, true, true,
                                             true);
      order++;
      break;
    }
    case 15: {
      companion->direction = Direction::LEFT;
      player->sprite = &player->atlas.sprites[14];

      dialog = {"Right..."};
      FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX,
                                             "Mary", "Martyr", dialog,
                                             false, true, true, false);
      order++;
      break;
    }
    case 16: {
      companion->lock_sprite = true;
      companion->sprite = &companion->atlas.sprites[12];

      dialog = {"Here, let me help you up."};
      FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX,
                                             "Erwin", "Lost Vagabond",
                                             dialog, false, true, false,
                                             false);
      order++;
      break;
    }
    case 17: {
      Game::fadeout(1.0);
      order++;
      break;
    }
    case 18: {
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

void DSErwinMild::draw() {
  if (black_out) {
    ClearBackground(BLACK);
  }
}
