#ifndef SURFACE_H_
#define SURFACE_H_

#include "SDL_surface.h"
#include "SDL_ttf.h"
#include <memory>

namespace sdl
{
class Surface
{
public:
  Surface(void);
  Surface(TTF_Font* font, char const* text, SDL_Color fg);
  Surface(char const* file);
  operator SDL_Surface*(void) const;
  SDL_Surface* operator->(void) const;
  
  ~Surface(void);
  Surface(Surface const& other);
  Surface(Surface&& other);
  Surface& operator=(Surface other);
private:
  std::shared_ptr<SDL_Surface> impl_;
};
}
//class Texture
//{
//public:
//  Texture(void)
//  {
//    impl_ = nullptr;
//  }
//  Texture(SDL_Renderer* renderer, SDL_Surface* surface)
//  {
//    impl_ = SDL_CreateTextureFromSurface(renderer, surface);
//    if(!impl_)
//    {
//      throw Exception();
//    }
//  }
//  ~Texture(void)
//  {
//    if(impl_)
//    {
//      SDL_DestroyTexture(impl_);
//    }
//  }
//  operator SDL_Texture*(void) const
//  {
//    return impl_;
//  }
//  SDL_Texture* operator->(void) const
//  {
//    return impl_;
//  }
//private:
//  SDL_Texture* impl_;
//};
//}
#endif