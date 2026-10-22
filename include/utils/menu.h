#include <array>
#include <unordered_set>
#include <cstddef>

namespace MenuUtils {
template<typename Type, size_t Num>
void nextOption(std::array<Type, Num> &options, 
                typename std::array<Type, Num>::iterator &selected,
                std::unordered_set<Type> *disallowed = NULL,
                bool loop = true) 
{
  if (options.size() == 1) {
    return;
  }

  selected++;

  bool at_end = selected == options.end();
  if (!loop && at_end) {
    selected--;
  }
  else if (at_end) {
    selected = options.begin();
  }

  if (disallowed == NULL) {
    return;
  }
  
  if (disallowed->find(*selected) == disallowed->end()) {
    return;
  }

  auto *next = selected + 1;
  bool last = next == options.end();
  if (loop || !last) {
    nextOption(options, selected, disallowed, loop);
  }
  else {
    selected--;
  }
}

template<typename Type, size_t Num>
void prevOption(std::array<Type, Num> &options, 
                typename std::array<Type, Num>::iterator &selected,
                std::unordered_set<Type> *disallowed = NULL,
                bool loop = true)
{
  if (options.size() == 1) {
    return;
  }

  bool at_begin = selected == options.begin();
  if (loop && at_begin) {
    selected = options.end();
  }

  at_begin = selected == options.begin();
  if (!at_begin) {
    selected--;
  }

  if (disallowed == NULL) {
    return;
  }

  if (disallowed->find(*selected) == disallowed->end()) {
    return;
  }

  bool first = selected == options.begin();
  auto *prev = selected - 1;
  if (loop || !first) {
    prevOption(options, selected, disallowed, loop);
  }
  else {
    selected++;
  } 
}

}
