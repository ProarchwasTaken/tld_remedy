#pragma once
#include <vector>

struct Animation {
  std::vector<int> frames;
  float frame_duration;

  float frame_clock = 0.0;
  std::vector<int>::iterator current = frames.begin();
};
