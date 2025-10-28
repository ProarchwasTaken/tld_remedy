#include "enums.h"
#include "game.h"
#include "base/party_member.h"
#include "base/status_effect.h"
#include "combat/status_effects/refreshed.h"
#include <raymath.h>
#include <plog/Log.h>


Refreshed::Refreshed(PartyMember *afflicted) : 
  StatusEffect(StatusID::REFRESHED, StatusType::POSITIVE, afflicted) 
{
  name = "Refreshed";
  persistent = false;
  this->afflicted = afflicted;

  float speed = afflicted->speed_multiplier;
  speed_gained = speed * 0.20;

  float recovery = afflicted->recovery;
  rec_gained = recovery * 0.20;

  PLOGI << "Speed to be gained: " << speed_gained;
  PLOGI << "Recovery to be gained: " << rec_gained;
}

void Refreshed::init(bool hide_text) {
  PLOGI << "Increasing the afflicted's speed and recovery by 20%";
  afflicted->speed_multiplier += speed_gained;
  afflicted->recovery += rec_gained;

  PLOGD << "Result: {SPD: " << afflicted->speed_multiplier << "| REC: " 
  << afflicted->recovery << "}";

  StatusEffect::init(hide_text);
  afflicted->sfx.play("refreshed_gain");
}

Refreshed::~Refreshed() {
  if (end) {
    PLOGI << "Undoing stat bonuses.";
    afflicted->speed_multiplier -= speed_gained;
    afflicted->recovery -= rec_gained;
    afflicted->sfx.play("refreshed_lost");
  }
}

void Refreshed::refresh() {
  PLOGI << "Refreshed the Refresh status effect that refreshes "
    "combatants in a refreshing matter.";
  effect_clock = 0.0;
}

void Refreshed::logic() {
  float *morale = &afflicted->morale;
  float max_morale = afflicted->max_morale;
  float recovery = afflicted->recovery;

  float magnitude = (max_morale * 0.035) * recovery;
  magnitude = magnitude * Game::deltaTime();

  *morale = Clamp(*morale + magnitude, 0.0, max_morale);

  effect_clock += Game::deltaTime() / effect_time;
  if (effect_clock >= 1.0) {
    end = true;
  }
}
