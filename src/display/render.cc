#include "render.h"
#include "sdl_exception.h"
#include "flood_fill.h"
#include "painter.h"
#include "SDL_sysrender.h"

namespace sdl
{
static int RenderCopyEx(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* srcrect, SDL_Rect const* dstrect, double angle)
{
  SDL_Rect real_srcrect = {0, 0, texture->w, texture->h};
  if(srcrect) 
  {
    if(!SDL_IntersectRect(srcrect, &real_srcrect, &real_srcrect)) 
    {
      return 0;
    }
  }

  SDL_FRect frect;
  if(dstrect) 
  {
    frect.x = dstrect->x;
    frect.y = dstrect->y;
    frect.w = dstrect->w;
    frect.h = dstrect->h;
  } 
  else 
  {
    SDL_Rect real_dstrect = {0, 0, 0, 0};
    SDL_RenderGetViewport(renderer, &real_dstrect);
    frect.x = real_dstrect.x;
    frect.y = real_dstrect.y;
    frect.w = real_dstrect.w;
    frect.h = real_dstrect.h;
  }

  SDL_FPoint fcenter;
  fcenter.x = .5f * frect.w;
  fcenter.y = .5f * frect.h;

  if(texture->native) 
  {
    texture = texture->native;
  }
  if(renderer->hidden) 
  {
    return 0;
  }
  return renderer->RenderCopyEx(renderer, texture, &real_srcrect, &frect, angle, &fcenter, SDL_FLIP_NONE);
}

void Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, double angle)
{
  if(RenderCopyEx(renderer, texture, source, destination, angle))
  {
    BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
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
    if(!adjusted.w)
    {
      adjusted.w = texture->w;
    }
    if(!adjusted.h)
    {
      adjusted.h = texture->h;
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