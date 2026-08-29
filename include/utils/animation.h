#pragma once
#include "data/animation.h"


namespace SpriteAnimation {
void play(Animation *&anim, Animation *next, bool loop);
void progress(Animation *&anim, bool loop);
}
