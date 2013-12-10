#ifndef TEXTURE_IMPL_H_
#define TEXTURE_IMPL_H_

#include "SDL_rect.h"
#include "SDL_render.h"
#include "bounding_box.h"
#include "window.h"
#include "render.h"

namespace display
{
class TextureImpl
{
public:
  TextureImpl(SDL_Texture* texture, SDL_Renderer* renderer, SDL_Rect* view, float* zoom);
  void Render(SDL_Rect const* source, SDL_Rect const* destination, float parallax, bool tile, double angle) const;
  
  ~TextureImpl(void);

  SDL_Texture* texture_;
  SDL_Renderer* renderer_;
  SDL_Rect* view_;
  float* zoom_;
};
}
#endif