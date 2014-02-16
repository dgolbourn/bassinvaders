#include "render.h"
#include "sdl_exception.h"
#include "flood_fill.h"
#include "painter.h"

namespace sdl
{
void Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, double angle)
{
  if(SDL_RenderCopyEx(renderer, texture, source, destination, angle, nullptr, SDL_FLIP_NONE))
  {
    throw sdl::Exception();
  }
}

static int Transform(int x, int new_origin, int width, float zoom, float parallax)
{
  float x0 = float(x);
  float w = .5f * float(width);
  x0 -= parallax * float(new_origin);
  x0 -= w;
  x0 *= zoom;
  x0 += w;
  return int(x0);
}

void Render(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, SDL_Point const* view, float zoom, float parallax, bool tile, double angle)
{
  if(destination)
  {
    SDL_Rect adjusted = *destination;

    if(!adjusted.w || !adjusted.h)
    {
      int w, h;
      if(SDL_QueryTexture(texture, nullptr, nullptr, &w, &h))
      {
        throw sdl::Exception();
      }
      if(!adjusted.w)
      {
        adjusted.w = w;
      }
      if(!adjusted.h)
      {
        adjusted.h = h;
      }
    }

    if(parallax > 0.f)
    {
      int w, h;
      SDL_GetWindowSize(window, &w, &h);
      adjusted.x = Transform(adjusted.x, view->x, w, zoom, parallax);
      adjusted.y = Transform(adjusted.y, view->y, h, zoom, parallax);
      adjusted.w = int(zoom * float(adjusted.w));
      adjusted.h = int(zoom * float(adjusted.h));
    }
    else
    {
      if((adjusted.x < 0) || (adjusted.y < 0))
      {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        if(adjusted.x < 0)
        {
          adjusted.x += w - adjusted.w;
        }
        if(adjusted.h < 0)
        {
          adjusted.y += h - adjusted.h;
        }
      }
    }
   
    if(tile)
    {
      algorithm::FloodFill<Painter>()(Painter(window, renderer, texture, source, &adjusted, nullptr, angle));
    }
    else
    {
      Render(renderer, texture, source, &adjusted, angle);
    }
  }
  else
  {
    Render(renderer, texture, source, nullptr, angle);
  }
}
}