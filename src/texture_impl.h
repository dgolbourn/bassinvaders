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

  void Render(void);
  void Render(BoundingBox& source, BoundingBox& destination);
  void Render(int x, int y);
  
  SDL_Texture* texture_;
  SDL_Renderer* renderer_;
};

}
#endif