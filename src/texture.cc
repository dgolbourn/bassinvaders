#include "texture.h"
#include "texture_impl.h"
#include "SDL_render.h"
#include "window_impl.h"
#include "bounding_box.h"
#include "bounding_box_impl.h"
#include "sdl_exception.h"

namespace display
{

TextureImpl::TextureImpl(SDL_Texture* texture, WindowImpl* window)
{
  reference_count_ = 1;
  texture_ = texture;
  window_ = window;
}

TextureImpl::~TextureImpl(void)
{
}

void TextureImpl::Free(void)
{
  if(reference_count_ > 0)
  {
    reference_count_--;
    if(reference_count_ == 0)
    {
      if(window_)
      {
        auto textureiter = window_->textures_.find(this);
        if(textureiter != window_->textures_.end())
	      {
          auto fileiter = window_->files_.find(textureiter->second);
          if(fileiter != window_->files_.end())
	        {
            window_->files_.erase(fileiter);
          }
          window_->textures_.erase(textureiter);
        }
      }
      SDL_DestroyTexture(texture_);
      delete this;
    }
  }
}

void TextureImpl::Render(int x, int y)
{
  if(window_)
  {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;

    if(SDL_QueryTexture(texture_, nullptr, nullptr, &rect.w, &rect.h))
    {
      throw sdl::Exception();
    }

    SDL_RenderCopy(window_->renderer_, texture_, nullptr, &rect);
  }
}

void TextureImpl::Render(void)
{
  if(window_)
  {
    SDL_RenderCopy(window_->renderer_, texture_, nullptr, nullptr);
  }
}

void TextureImpl::Render(BoundingBox& source, BoundingBox& destination)
{
  if(window_)
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

    SDL_RenderCopy(window_->renderer_, texture_, 
                   source_rect, destination_rect);
  }
}

Texture::Texture(void)
{
  impl_ = nullptr;
}

Texture::Texture(const Texture& original)
{
  impl_ = original.impl_;
  if(impl_)
  {
    impl_->reference_count_++;
  }
}

Texture::Texture(Texture&& original)
{
  impl_ = original.impl_;
  original.impl_ = nullptr;
}

Texture::~Texture(void)
{
  if(impl_)
  {
    impl_->Free();
  }
  impl_ = nullptr;
}

Texture& Texture::operator=(Texture original)
{
  std::swap(impl_, original.impl_);
  return *this;
}

void Texture::Render(int x, int y)
{
  if(impl_)
  {
    impl_->Render(x, y);
  }
}

void Texture::Render(void)
{
  if(impl_)
  {
    impl_->Render();
  }
}

void Texture::Render(BoundingBox& source, BoundingBox& destination)
{
  if(impl_)
  {
    impl_->Render(source, destination);
  }
}

}