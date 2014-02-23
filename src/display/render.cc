#include "render.h"
#include "sdl_exception.h"
#include "flood_fill.h"
#include "painter.h"
#include "SDL_sysrender.h"
namespace sdl
{
void Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source_ptr, SDL_Rect const* dstrect, double angle)
{
  SDL_FRect destination;
  destination.x = dstrect->x;
  destination.y = dstrect->y;
  destination.w = dstrect->w;
  destination.h = dstrect->h;

  SDL_FPoint centre;
  centre.x = .5f * destination.w;
  centre.y = .5f * destination.h;

  if(texture->native) 
  {
    texture = texture->native;
  }

  if(renderer->hidden == SDL_FALSE)
  {
    if(renderer->RenderCopyEx(renderer, texture, source_ptr, &destination, angle, &centre, SDL_FLIP_NONE))
    {
      BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
    }
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

void Render(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source_ptr, SDL_Rect const* destination_ptr, SDL_Point const* view, float zoom, float parallax, bool tile, double angle)
{
  bool render = true;
  SDL_Rect source = {0, 0, texture->w, texture->h};
  if(source_ptr)
  {
    if(SDL_IntersectRect(source_ptr, &source, &source) == SDL_FALSE)
    {
      render = false;
    }
  }

  if(render)
  {
    SDL_Rect destination;
    if(destination_ptr)
    {
      destination = *destination_ptr;
      if(!destination.w)
      {
        destination.w = texture->w;
      }
      if(!destination.h)
      {
        destination.h = texture->h;
      }

      if(parallax > 0.f)
      {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        destination.x = Transform(destination.x, view->x, w, zoom, parallax);
        destination.y = Transform(destination.y, view->y, h, zoom, parallax);
        destination.w = int(zoom * float(destination.w));
        destination.h = int(zoom * float(destination.h));
      }
      else
      {
        if((destination.x < 0) || (destination.y < 0))
        {
          int w, h;
          SDL_GetWindowSize(window, &w, &h);
          if(destination.x < 0)
          {
            destination.x += w - destination.w;
          }
          if(destination.h < 0)
          {
            destination.y += h - destination.h;
          }
        }
      }
    }
    else
    {
      destination.x = 0;
      destination.y = 0;
      SDL_GetWindowSize(window, &destination.w, &destination.h);
    }
   
    if(tile)
    {
      algorithm::FloodFill<Painter>()(Painter(window, renderer, texture, &source, &destination, angle));
    }
    else
    {
      Render(renderer, texture, &source, &destination, angle);
    }
  }
}
}