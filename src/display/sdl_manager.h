#ifndef SDL_MANAGER_H_
#define SDL_MANAGER_H_

#include "SDL_stdinc.h"

namespace sdl
{
class Library
{
public:
  Library(Uint32 flags);
  ~Library(void);
private:
  Uint32 const flags_;
};
}
#endif