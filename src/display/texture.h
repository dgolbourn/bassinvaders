#ifndef SDL_TEXTURE_H_
#define SDL_TEXTURE_H_

#include "SDL_render.h"
#include <memory>
#include "weak_ptr.h"

namespace sdl
{
class Texture
{
public:
  Texture(void);
  Texture(SDL_Renderer* renderer, SDL_Surface* surface);
  operator SDL_Texture*(void) const;
  SDL_Texture* operator->(void) const;
  explicit operator bool(void) const;

  ~Texture(void);
  Texture(Texture const& other);
  Texture(Texture&& other);
  Texture& operator=(Texture other);
  typedef memory::WeakPtr<class Texture, SDL_Texture> WeakPtr;
private:
  std::shared_ptr<SDL_Texture> impl_;
  friend WeakPtr;
};
}
#endif