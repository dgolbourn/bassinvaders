#ifndef TEXTURE_IMPL_H_
#define TEXTURE_IMPL_H_

#include "SDL_rect.h"
#include "SDL_render.h"
#include "bounding_box.h"
#include "window.h"

namespace display
{

class TextureImpl
{
public:
  TextureImpl(SDL_Texture* texture, SDL_Renderer* renderer);
  ~TextureImpl(void);

  void Render(void) const;
  void Render(BoundingBox const& source, BoundingBox const& destination) const;
  void Render(int x, int y) const;
  
  SDL_Texture* texture_;
  SDL_Renderer* renderer_;
};

}
#endif