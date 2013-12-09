#ifndef RENDER_H_
#define RENDER_H_

#include "SDL_render.h"

namespace sdl
{
class View
{
public:
  float x_;
  float y_;
  float half_width_;
  float half_height_;
  float zoom_;
  int width_;
  int height_;
};

void Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, View const& view, float parallax, bool tile, double angle);
}
#endif