#include "texture.h"
#include "texture_impl.h"
#include "render.h"

namespace display
{
void TextureImpl::Render(void) const
{
  sdl::Render(renderer_, texture_, nullptr, nullptr, sdl::View(), 0.f, false, 0.);
}

void TextureImpl::Render(SDL_Rect const* source, SDL_Rect const* destination, float parallax, bool tile, double angle) const
{
  sdl::Render(renderer_, texture_, source, destination, view_, parallax, tile, angle);
}

Texture::Texture(void)
{
}

TextureImpl::TextureImpl(SDL_Texture* texture, SDL_Renderer* renderer, sdl::View& view) : texture_(texture), renderer_(renderer), view_(view)
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

void Texture::Render(void) const
{
  impl_->Render();
}

void Texture::Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle) const
{
  impl_->Render(source, destination, parallax, tile, angle);
}
}