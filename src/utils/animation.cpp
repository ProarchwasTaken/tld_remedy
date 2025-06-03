#include "game.h"
#include "data/animation.h"
#include "utils/animation.h"


void SpriteAnimation::play(Animation &animation, bool loop) {
  animation.frame_clock += Game::time() / animation.frame_duration;

  if (animation.frame_clock < 1.0) {
    return;
  }

  animation.current++;

  if (loop && animation.current == animation.frames.end()) {
    animation.current = animation.frames.begin();
  }
  else {
    animation.current--;
  }
}
