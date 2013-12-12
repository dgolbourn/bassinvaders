#ifndef TEXTURE_IMPL_H_
#define TEXTURE_IMPL_H_

#include "SDL_rect.h"
#include "SDL_render.h"
#include "bounding_box.h"
#include "window.h"
#include "render.h"
#include "sdl_texture.h"

namespace display
{
class TextureImpl
{
public:
  TextureImpl(sdl::Texture const& texture, SDL_Window* window, SDL_Renderer* renderer, SDL_Point* view, float* zoom);
  void Render(SDL_Rect const* source, SDL_Rect const* destination, float parallax, bool tile, double angle) const;
  
  ~TextureImpl(void);

  sdl::Texture texture_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;
  SDL_Point* view_;
  float* zoom_;
};
}
#endif