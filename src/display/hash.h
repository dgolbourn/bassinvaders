#ifndef HASH_H_
#define HASH_H_
#include <functional>

namespace algorithm
{
inline std::size_t HashCombine(std::size_t const& seed1, std::size_t const& seed2)
{
  return seed1 ^ (seed2 + 0x9E3779B9 + (seed1 << 6) + (seed1 >> 2));
}

template<class a, class b> class Hash
{
public:
  size_t operator()(std::pair<a, b> const& pair) const
  {
    return HashCombine(std::hash<a>()(pair.first), std::hash<b>()(pair.second));
  }
};
}
#endif