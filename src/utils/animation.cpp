#include "game.h"
#include "data/animation.h"
#include "utils/animation.h"
#include <plog/Log.h>


void SpriteAnimation::play(Animation &animation, bool loop) {
  animation.frame_clock += Game::time() / animation.frame_duration;

  if (animation.frame_clock < 1.0) {
    return;
  }

  animation.frame_clock = 0.0;
  animation.current++;

  if (animation.current != animation.frames.end()) {
    return;
  }

  if (loop) {
    animation.current = animation.frames.begin();
  }
  else {
    animation.current--;
  }
}
