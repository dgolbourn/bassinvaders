#include "texture.h"
#include "texture_impl.h"
#include "SDL_render.h"
#include "bounding_box.h"
#include "sdl_exception.h"

namespace display
{
TextureImpl::TextureImpl(SDL_Texture* texture, SDL_Renderer* renderer, View& view) : texture_(texture), renderer_(renderer), view_(view)
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

static void RenderCopyEx(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination)
{
  if(SDL_RenderCopyEx(renderer, texture, source, destination, 0., nullptr, SDL_FLIP_NONE))
  {
    throw sdl::Exception();
  }
}

static void RenderCopy(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, View const& view, float parallax, bool tile)
{
  if(destination)
  {
    SDL_Rect adjusted;
    if(parallax > 0.f)
    {
      adjusted.x = transform(destination->x, view.x_, view.half_width_, view.zoom_, parallax);
      adjusted.y = transform(destination->y, view.y_, view.half_height_, view.zoom_, parallax);
      adjusted.w = transform(destination->w, 0.f, 0.f, view.zoom_, 0.f);
      adjusted.h = transform(destination->h, 0.f, 0.f, view.zoom_, 0.f);
    }
    else
    { 
      adjusted = *destination;
    }
    if(tile)
    {
      int const width = abs(adjusted.w);
      int const height = abs(adjusted.h);
      adjusted.x %= width;
      adjusted.x -= width;
      adjusted.y %= height;
      adjusted.y -= height;
      int const ystart = adjusted.y;
      while(adjusted.x < view.width_)
      {
        adjusted.y = ystart;
        while(adjusted.y < view.height_)
        {
          RenderCopyEx(renderer, texture, source, &adjusted);
          adjusted.y += height;
        }
        adjusted.x += width;
      }
    }
    else
    {
      RenderCopyEx(renderer, texture, source, &adjusted);
    }
  }
  else
  {
    RenderCopyEx(renderer, texture, source, destination);
  }
}

void TextureImpl::Render(void) const
{
  RenderCopy(renderer_, texture_, nullptr, nullptr, View(), 0.f, false);
}

void TextureImpl::Render(SDL_Rect const* source, SDL_Rect const* destination, float parallax, bool tile) const
{
  RenderCopy(renderer_, texture_, source, destination, view_, parallax, tile);
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

void Texture::Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile) const
{
  impl_->Render(source, destination, parallax, tile);
}
}