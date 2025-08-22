#include "enums.h"
#include "game.h"
#include "base/party_member.h"
#include "base/status_effect.h"
#include "combat/status_effects/despondent.h"


Despondent::Despondent(PartyMember *afflicted) : 
  StatusEffect(StatusID::DESPONDENT, StatusType::NEGATIVE, afflicted)
{
  name = "Despondent";
  persistant = false;
  this->afflicted = afflicted;
}

void Despondent::init() {
  afflicted->demoralized = true;
}

Despondent::~Despondent() {
  afflicted->demoralized = false;
  afflicted->sfx.play("despondent_recover");
}

void Despondent::logic() {
  if (delay_clock <= 1.0) {
    delay_clock += Game::deltaTime() / delay_time;
    return;
  }

  float recovery = afflicted->recovery;
  float magnitude = (afflicted->max_morale * 0.035) * recovery;
  afflicted->morale += magnitude * Game::deltaTime();

  float init_morale = afflicted->init_morale;
  if (afflicted->morale >= init_morale) {
    end = true;
  }
}
