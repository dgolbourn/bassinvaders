#include "texture.h"
#include "texture_impl.h"
#include "SDL_render.h"
#include "bounding_box.h"
#include "sdl_exception.h"
#include "canvas.h"

namespace display
{
TextureImpl::TextureImpl(SDL_Texture* texture, SDL_Renderer* renderer, SDL_Window* window, View& view) : texture_(texture), window_(window), renderer_(renderer), view_(view)
{
}

TextureImpl::~TextureImpl(void)
{
  SDL_DestroyTexture(texture_);
}

static inline int transform(int x, float new_origin, float half_width, float zoom, float parallax)
{
  float x0 = float(x);
  x0 -= parallax * new_origin;
  x0 -= half_width;
  x0 *= zoom;
  x0 += half_width;
  return int(x0);
}

static void RenderCopyEx(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, double angle)
{
  if(SDL_RenderCopyEx(renderer, texture, source, destination, angle, nullptr, SDL_FLIP_NONE))
  {
    throw sdl::Exception();
  }
}

static void RenderTile(SDL_Renderer* renderer, SDL_Window* window, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect* destination, View const& view, double angle)
{
  sdl::Canvas canvas;
  if(angle)
  {
    canvas = sdl::Canvas(renderer, window, view.width_, view.height_);
    canvas.Lock();
  }

  int const width = abs(destination->w);
  int const height = abs(destination->h);
  destination->x %= width;
  destination->x -= width;
  destination->y %= height;
  destination->y -= height;
  int const ystart = destination->y;
  while(destination->x < view.width_)
  {
    destination->y = ystart;
    while(destination->y < view.height_)
    {
      RenderCopyEx(renderer, texture, source, destination, 0.);
      destination->y += height;
    }
    destination->x += width;
  }

  if(angle)
  {
    canvas.Unlock();
    SDL_Rect rect = { 0, 0, view.width_, view.height_ };
    canvas.Render(&rect, angle);
  }
}

static void RenderCopy(SDL_Renderer* renderer, SDL_Window* window, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, View const& view, float parallax, bool tile, double angle)
{
  if(destination)
  {
    if(parallax > 0.f)
    {
      SDL_Rect adjusted;
      adjusted.x = transform(destination->x, view.x_, view.half_width_, view.zoom_, parallax);
      adjusted.y = transform(destination->y, view.y_, view.half_height_, view.zoom_, parallax);
      adjusted.w = transform(destination->w, 0.f, 0.f, view.zoom_, 0.f);
      adjusted.h = transform(destination->h, 0.f, 0.f, view.zoom_, 0.f);
      if(tile)
      {
        RenderTile(renderer, window, texture, source, &adjusted, view, angle);
      }
      else
      {
        RenderCopyEx(renderer, texture, source, &adjusted, angle);
      }
    }
    else
    { 
      if(tile)
      {
        SDL_Rect adjusted = *destination;
        RenderTile(renderer, window, texture, source, &adjusted, view, angle);
      }
      else
      {
        RenderCopyEx(renderer, texture, source, destination, angle);
      }
    }
  }
  else
  {
    RenderCopyEx(renderer, texture, source, nullptr, angle);
  }
}

void TextureImpl::Render(void) const
{
  RenderCopy(renderer_, window_, texture_, nullptr, nullptr, View(), 0.f, false, 0.);
}

void TextureImpl::Render(SDL_Rect const* source, SDL_Rect const* destination, float parallax, bool tile, double angle) const
{
  RenderCopy(renderer_, window_, texture_, source, destination, view_, parallax, tile, angle);
}

Texture::Texture(void)
{
}

Texture::Texture(Texture const& other) : impl_(other.impl_)
{
}

Texture::Texture(Texture&& other) : impl_(std::move(other.impl_))
{
}

Texture::~Texture(void)
{
}

Texture& Texture::operator=(Texture other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

void Texture::Render(void) const
{
  impl_->Render();
}

void Texture::Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle) const
{
  impl_->Render(source, destination, parallax, tile, angle);
}
}