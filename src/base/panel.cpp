#include <cassert>
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "base/panel.h"


void Panel::transitionLogic() {
  assert(state != PanelState::READY);
  clock += Game::deltaTime() / transition_time;
  clock = Clamp(clock, 0.0, 1.0);

  if (clock != 1.0) {
    return;
  }

  if (state == PanelState::OPENING) {
    clock = 0.0;
  }
  else {
    terminate = true;
  }

  state = PanelState::READY;
}
