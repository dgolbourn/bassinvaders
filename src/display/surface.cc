#include "surface.h"
#include "SDL_image.h"
#include "ttf_exception.h"
#include "img_exception.h"
#include "sdl_exception.h"

namespace sdl
{
static void FreeSurface(SDL_Surface* ptr)
{
  if(ptr)
  {
    SDL_FreeSurface(ptr);
  }
}

Surface::Surface(TTF_Font* font, char const* text, SDL_Color fg)
{
  SDL_Surface* impl = nullptr;
  try
  {
    impl = TTF_RenderText_Blended(font, text, fg);
    if(!impl)
    {
      throw ttf::Exception();
    }
    impl_ = std::shared_ptr<SDL_Surface>(impl, FreeSurface);
  }
  catch(...)
  {
    if(!impl_)
    {
      FreeSurface(impl);
    }
    throw;
  }
}

Surface::Surface(char const* file)
{
  SDL_Surface* impl = nullptr;
  try
  {
    impl = IMG_Load(file);
    if(!impl)
    {
      throw img::Exception();
    }
    impl_ = std::shared_ptr<SDL_Surface>(impl, FreeSurface);
  }
  catch(...)
  {
    if(!impl_)
    {
      FreeSurface(impl);
    }
    throw;
  }
}

Surface::operator SDL_Surface*(void) const
{
  return impl_.get();
}

SDL_Surface* Surface::operator->(void) const
{
  return impl_.get();
}
}