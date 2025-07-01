#include <cassert>
#include <cstddef>
#include "game.h"
#include "data/animation.h"
#include "utils/animation.h"
#include <plog/Log.h>


void SpriteAnimation::play(Animation *&anim, Animation *next, bool loop) {
  bool new_animation = anim != next && next != NULL;
  if (new_animation) {
    anim = next;
    anim->current = anim->frames.begin();
    anim->frame_clock = 0.0;
  }

  assert(!anim->frames.empty());
  if (anim->frames.size() == 1) {
    return;
  }

  anim->frame_clock += Game::time() / anim->frame_duration;

  if (anim->frame_clock < 1.0) {
    return;
  }

  anim->frame_clock = 0.0;
  anim->current++;

  if (anim->current != anim->frames.end()) {
    return;
  }

  if (loop) {
    anim->current = anim->frames.begin();
  }
  else {
    anim->current--;
  }
}
