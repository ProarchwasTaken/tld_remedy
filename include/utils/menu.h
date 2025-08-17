#include <array>
#include <unordered_set>
#include <cstddef>

namespace MenuUtils {
template<typename Type, size_t Num>
void nextOption(std::array<Type, Num> &options, 
                typename std::array<Type, Num>::iterator &selected,
                std::unordered_set<Type> &disallowed) 
{
  selected++;

  if (selected == options.end()) {
    selected = options.begin();
  }

  if (disallowed.find(*selected) != disallowed.end()) {
    selected++;
  }

}

template<typename Type, size_t Num>
void prevOption(std::array<Type, Num> &options, 
                typename std::array<Type, Num>::iterator &selected,
                std::unordered_set<Type> &disallowed)
{
  if (selected == options.begin()) {
    selected = options.end();
  }

  selected--;

  if (disallowed.find(*selected) != disallowed.end()) {
    selected--;
  }
}

}
