#ifndef SDL_TEXTURE_H_
#define SDL_TEXTURE_H_

#include "SDL_render.h"
#include <memory>

namespace sdl
{
class Texture
{
public:
  Texture(void);
  Texture(SDL_Renderer* renderer, SDL_Surface* surface);
  operator SDL_Texture*(void) const;
  SDL_Texture* operator->(void) const;

  ~Texture(void);
  Texture(Texture const& other);
  Texture(Texture&& other);
  Texture& operator=(Texture other);
private:
  std::shared_ptr<SDL_Texture> impl_;
};
}
#endif