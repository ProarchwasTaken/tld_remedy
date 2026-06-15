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
      dialog = {
        "r...y...!",
        "M..a..r..y..!"
      };
      FieldHandler::raise<OpenDialogEvent>(FieldEVT::OPEN_DIALOG, dialog, 
                                           false, false, true, true, 
                                           0.5f);
      seq_clock = 0.0;
      order++;
      break;
    }  
    case 1: {
      if (seq_clock == 1.0) {
        dialog = {"Uuuuugggghhh...."};
        FieldHandler::raise<OpenDialogEventEx>(FieldEVT::OPEN_DIALOG_EX, 
                                               "Mary", "Martyr", dialog, 
                                               false, false, true, true, 
                                               0.25f);
        seq_time = 0.25;
        seq_clock = 0.0;
        order++;
      }

      break;
    }
    case 2: {
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
    case 3: {
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
    case 4: {
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
    case 5: {
      break;
    }
    case 6: {
      Game::fadeout(0.5);
      order++;
      break;
    }
    case 7: {
      PLOGD << "Ending Sequence.";
      PlayerActor::setControllable(true);
      player->lock_sprite = false;

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
