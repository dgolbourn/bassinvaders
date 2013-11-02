#include "texture.h"
#include "texture_impl.h"
#include "SDL_render.h"
#include "bounding_box.h"
#include "bounding_box_impl.h"
#include "sdl_exception.h"

namespace display
{

TextureImpl::TextureImpl(SDL_Texture* texture, SDL_Renderer* renderer)
{
  texture_ = texture;
  renderer_ = renderer;
}

TextureImpl::~TextureImpl(void)
{
  SDL_DestroyTexture(texture_);
}

void TextureImpl::Render(int x, int y) const
{
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;

  if(SDL_QueryTexture(texture_, nullptr, nullptr, &rect.w, &rect.h))
  {
    throw sdl::Exception();
  }

  SDL_RenderCopy(renderer_, texture_, nullptr, &rect);
}

void TextureImpl::Render(void) const
{
  SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
}

void TextureImpl::Render(BoundingBox const& source, BoundingBox const& destination) const
{
  SDL_Rect* source_rect = nullptr;
  if(source.impl_)
  {
    source_rect = &source.impl_->rect_;
  }

  SDL_Rect* destination_rect = nullptr;
  if(destination.impl_)
  {
    destination_rect = &destination.impl_->rect_;
  }

  SDL_RenderCopy(renderer_, texture_, source_rect, destination_rect);
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

void Texture::Render(int x, int y) const
{
  impl_->Render(x, y);
}

void Texture::Render(void) const
{
  impl_->Render();
}

void Texture::Render(BoundingBox const& source, BoundingBox const& destination) const
{
  impl_->Render(source, destination);
}

}