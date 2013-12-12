#include "sdl_texture.h"
#include "sdl_exception.h"

namespace sdl
{
class Deleter
{
public:
  void operator()(SDL_Texture* impl)
  {
    if(impl)
    {
      SDL_DestroyTexture(impl);
    }
  }
};

Texture::Texture(void)
{
}

Texture::Texture(SDL_Renderer* renderer, SDL_Surface* surface)
{
  SDL_Texture* impl = nullptr;
  try
  {
    impl = SDL_CreateTextureFromSurface(renderer, surface);
    if(!impl)
    {
      throw Exception();
    }
    impl_ = std::shared_ptr<SDL_Texture>(impl, Deleter());
  }
  catch(...)
  {
    if(!impl_)
    {
      Deleter().operator()(impl);
    }
    throw;
  }
}

Texture::operator SDL_Texture*(void) const
{
  return impl_.get();
}

SDL_Texture* Texture::operator->(void) const
{
  return impl_.get();
}

Texture::~Texture(void)
{
}

Texture::Texture(Texture const& other) : impl_(other.impl_)
{
}

Texture::Texture(Texture&& other) : impl_(std::move(other.impl_))
{
}

Texture& Texture::operator=(Texture other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}