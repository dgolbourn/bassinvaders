#include "texture.h"
#include "texture_impl.h"
#include "SDL_render.h"
#include "window_impl.h"
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

void TextureImpl::Render(int x, int y)
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

void TextureImpl::Render(void)
{
  SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
}

void TextureImpl::Render(BoundingBox& source, BoundingBox& destination)
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

Texture::Texture(const Texture& original)
{
  impl_ = original.impl_;
}

Texture::Texture(Texture&& original)
{
  impl_ = original.impl_;
  original.impl_.reset();
}

Texture::~Texture(void)
{
}

Texture& Texture::operator=(Texture original)
{
  std::swap(impl_, original.impl_);
  return *this;
}

void Texture::Render(int x, int y)
{
  impl_->Render(x, y);
}

void Texture::Render(void)
{
  impl_->Render();
}

void Texture::Render(BoundingBox& source, BoundingBox& destination)
{
  impl_->Render(source, destination);
}

}