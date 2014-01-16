#include "texture.h"
#include "sdl_exception.h"

namespace sdl
{
static void FreeTexture(SDL_Texture* impl)
{
  if(impl)
  {
    SDL_DestroyTexture(impl);
  }
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
    impl_ = std::shared_ptr<SDL_Texture>(impl, FreeTexture);
  }
  catch(...)
  {
    if(!impl_)
    {
      FreeTexture(impl);
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

Texture::operator bool(void) const
{
  return bool(impl_);
}
}