#include <random>
#include <string>
#include <array>
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include "enums.h"
#include "game.h"
#include "base/field_sequence.h"
#include "data/session.h"
#include "data/field_event.h"
#include "scenes/field.h"
#include "field/actors/player.h"
#include "field/system/field_handler.h"
#include "field/sequences/incap.h"
#include <plog/Log.h>

using std::uniform_int_distribution, std::string, std::array;


IncapSequence::IncapSequence(Session *session) : 
  FieldSequence("Incapacitated Sequence", SequenceID::INCAP)
{
  Companion *companion = &session->companion;
  setupIncapTexture(companion->companion_id);

  text = getIncapMessage();
  text_color = Game::palette[2];
  text_color.a = 0;

  player_injury = session->player.injury;
  companion_id = session->companion.companion_id;

  applySupplyPenalty(session);
  applyCompanionDamage(&session->companion);

  PlayerActor::setControllable(false);
  Game::noise->setTint(Game::palette[2]);
  Game::noise->setAlpha(0.15);
  Game::bgm->stop();
}

IncapSequence::~IncapSequence() {
  UnloadTexture(texture);
}

void IncapSequence::applySupplyPenalty(Session *session) {
  float m_recovery = session->player.recovery;
  float m_modifier = 0.85 - m_recovery;
  PLOGD << "Mary REC Modifier: " << m_modifier;

  float c_recovery = session->companion.recovery;
  float c_modifier = 0.90 - c_recovery;
  PLOGD << "Companion REC Modifier: " << c_modifier;

  float percentage = 0.50 + m_modifier + c_modifier;
  percentage = Clamp(percentage, 0.0, 1.0);
  PLOGD << "Final Penalty: " << percentage * 100 << "%";

  float penalty = session->supplies * percentage;
  penalty = std::ceilf(penalty);
  PLOGI << "Lost " << penalty << " Supplies.";

  session->supplies -= penalty;
  if (session->supplies < 0) {
    session->supplies = 0;
  }
}

void IncapSequence::applyCompanionDamage(Companion *companion) {
  float max_life = companion->max_life;
  float resilience = companion->resilience;

  float percentage = Clamp(1.0 - resilience, 0.0, 1.0);
  PLOGD << "Percentage: " << percentage * 100 << "%";

  float damage = max_life * percentage;
  PLOGI << "Damage taken: " << damage;

  companion->life = Clamp(companion->life - damage, 1.0, max_life);
  companion->life = std::floorf(companion->life);
  PLOGI << "Resulting Life: " << companion->life;
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

string IncapSequence::getIncapMessage() {
  array<string, 6> message_pool = {
    "Everything hurts.",
    "I can't move.",
    "So much red...",
    "I don't want to die..",
    "So... tired..",
    "Losing consciousness.."
  };

  uniform_int_distribution<int> range(0, 5);
  int index = range(Game::RNG);

  string selected = message_pool.at(index);
  return selected;
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
      text_color.a = Lerp(0, 255, seq_clock);

      if (seq_clock == 1.0) {
        PLOGD << "Hiding everything.";
        Game::noise->setTint(WHITE);
        Game::noise->setAlpha(0);

        tint.a = 0;
        text_color.a = 0;

        seq_clock = 0;
        seq_time = 3.0;

        order++;
      }
      break;
    }
    case 3: {
      if (seq_clock == 1.0) {
        followUpSequence();
        order++;
      }
      break;
    }
  }
}

void IncapSequence::followUpSequence() {
  PLOGI << "Attempting to trigger follow up sequence.";
  bool mild_injury = player_injury < 7;

  switch (companion_id) {
    case CompanionID::ERWIN: {
      FieldEVT event_id = FieldEVT::START_SEQUENCE;
      SequenceID sequence_id;
      if (mild_injury) {
        PLOGD << "Raising an event for the mild injury sequence.";
        sequence_id = SequenceID::DS_ERWIN_MILD;
      }
      else {
        PLOGD << "Raising an event for the severe injury sequence.";
        sequence_id = SequenceID::DS_ERWIN_SEVERE;
      }

      FieldHandler::raise<StartSequenceEvent>(event_id, sequence_id);
      break;
    }
  }
}

void IncapSequence::draw() {
  ClearBackground(BLACK);
  DrawTextureV(texture, position, tint);

  Font *font = &Game::sm_font;
  int text_size = font->baseSize;
  DrawTextEx(*font, text.c_str(), {16, 16}, text_size, 0, text_color);
}
