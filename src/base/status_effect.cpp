#include <string>
#include <raylib.h>
#include "enums.h"
#include "game.h"
#include "base/combatant.h"
#include "base/status_effect.h"
#include "combat/system/evt_handler.h"

using std::string;


StatusEffect::StatusEffect(StatusID id, StatusType type, 
                           Combatant *afflicted) 
{
  this->id = id;
  this->type = type;
  this->afflicted = afflicted;
}

StatusEffect::~StatusEffect() {
  if (!end)  {
    return;
  }
  
  string text = "-" + name;
  
  Color color;
  if (type == StatusType::NEGATIVE) {
    color = Game::palette[14];
  }
  else {
    color = Game::palette[2];
  }

  CombatHandler::raise<CreateStatTxtCB>(CombatEVT::CREATE_STAT_TXT, 
                                        afflicted, text, color);
}

void StatusEffect::init(bool hide_text) {
  string text = "+" + name;

  Color color;
  if (type == StatusType::NEGATIVE) {
    color = Game::palette[33];
  }
  else {
    color = Game::palette[14];
  }

  if (!hide_text) {
    CombatHandler::raise<CreateStatTxtCB>(CombatEVT::CREATE_STAT_TXT, 
                                          afflicted, text, color);
  }
}
