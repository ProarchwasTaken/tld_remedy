#pragma once
#include <memory>
#include <raylib.h>
#include "data/combatant_event.h"
#include "base/party_member.h"


class LifeHud {
public:
  LifeHud(Vector2 position);
  ~LifeHud();

  void assign(PartyMember *user);

  void evaluateEvent(std::unique_ptr<CombatantEvent> &event);
  void update();
  void draw();
};
