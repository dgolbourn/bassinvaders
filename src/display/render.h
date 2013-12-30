#ifndef RENDER_H_
#define RENDER_H_

#include "SDL_render.h"

namespace sdl
{
void Render(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, SDL_Point const* view, float zoom, float parallax, bool tile, double angle);
void Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, double angle);
}
#endif