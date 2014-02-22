#ifndef SURFACE_H_
#define SURFACE_H_

#include "SDL_surface.h"
#include "SDL_ttf.h"
#include <memory>

namespace sdl
{
class Surface
{
public:
  Surface(void) = default;
  Surface(TTF_Font* font, char const* text, SDL_Color colour);
  Surface(char const* file);
  operator SDL_Surface*(void) const;
  SDL_Surface* operator->(void) const;
private:
  std::shared_ptr<SDL_Surface> impl_;
};
}
#endif