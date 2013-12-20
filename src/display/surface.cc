#include "surface.h"
#include "SDL_image.h"
#include "ttf_exception.h"
#include "img_exception.h"
#include "sdl_exception.h"

namespace sdl
{
class Deleter
{
public:
  void operator()(SDL_Surface* impl)
  {
    if(impl)
    {
      SDL_FreeSurface(impl);
    }
  }
};

Surface::Surface(void)
{
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
    impl_ = std::shared_ptr<SDL_Surface>(impl, Deleter());
  }
  catch(...)
  {
    if(!impl_)
    {
      Deleter()(impl);
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
    impl_ = std::shared_ptr<SDL_Surface>(impl, Deleter());
  }
  catch(...)
  {
    if(!impl_)
    {
      Deleter()(impl);
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

Surface::~Surface(void)
{
}

Surface::Surface(Surface const& other) : impl_(other.impl_)
{
}

Surface::Surface(Surface&& other) : impl_(std::move(other.impl_))
{
}

Surface& Surface::operator=(Surface other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}