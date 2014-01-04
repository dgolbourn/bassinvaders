#ifndef SDL_LIBRARY_H_
#define SDL_LIBRARY_H_

#include "SDL_stdinc.h"

namespace sdl
{
class Library
{
public:
  Library(Uint32 flags);
  Library::Library(Library const& other);
  Library::Library(Library&& other);
  Library& Library::operator=(Library other);
  ~Library(void);
private:
  Uint32 flags_;
};
}
#endif