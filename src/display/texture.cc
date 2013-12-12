#include "texture.h"
#include "texture_impl.h"
#include "render.h"

namespace display
{
void TextureImpl::Render(SDL_Rect const* source, SDL_Rect const* destination, float parallax, bool tile, double angle) const
{
  sdl::Render(window_, renderer_, texture_, source, destination, view_, *zoom_, parallax, tile, angle);
}

Texture::Texture(void)
{
}

TextureImpl::TextureImpl(sdl::Texture const& texture, SDL_Window* window, SDL_Renderer* renderer, SDL_Point* view, float* zoom) : texture_(texture), window_(window), renderer_(renderer), view_(view), zoom_(zoom)
{
}

TextureImpl::~TextureImpl(void)
{
  SDL_DestroyTexture(texture_);
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

void Texture::Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle) const
{
  if(auto impl = impl_.lock())
  {
    impl->Render(source, destination, parallax, tile, angle);
  }
}
}