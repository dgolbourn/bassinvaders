#include "texture.h"
#include "texture_impl.h"
#include "render.h"

namespace display
{
void TextureImpl::Render(SDL_Rect const* source, SDL_Rect const* destination, float parallax, bool tile, double angle) const
{
  sdl::Render(renderer_, texture_, source, destination, view_, *zoom_, parallax, tile, angle);
}

Texture::Texture(void)
{
}

TextureImpl::TextureImpl(SDL_Texture* texture, SDL_Renderer* renderer, SDL_Rect* view, float* zoom) : texture_(texture), renderer_(renderer), view_(view), zoom_(zoom)
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
  impl_->Render(source, destination, parallax, tile, angle);
}
}