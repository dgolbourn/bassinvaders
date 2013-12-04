#include "canvas.h"
#include "sdl_exception.h"

namespace sdl
{
class CanvasImpl
{
public:
  CanvasImpl(SDL_Renderer* renderer, SDL_Window* window, int w, int h);
  void Lock(void);
  void Unlock(void);
  void Render(SDL_Rect const* destination, double angle);
  ~CanvasImpl(void);

  int w_;
  int h_;
  SDL_Texture* target_;
  SDL_BlendMode blend_mode_;
  Uint8 r_, g_, b_, a_;
  SDL_Renderer* renderer_;
  bool locked_;
};

CanvasImpl::CanvasImpl(SDL_Renderer* renderer, SDL_Window* window, int w, int h)
{
  locked_ = false;
  renderer_ = renderer;
  if(SDL_GetRenderDrawBlendMode(renderer, &blend_mode_))
  {
    throw sdl::Exception();
  }
  if(SDL_GetRenderDrawColor(renderer, &r_, &g_, &b_, &a_))
  {
    throw sdl::Exception();
  }
  target_ = SDL_CreateTexture(renderer_, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, w, h);
  if(!target_)
  {
    throw sdl::Exception();
  }
  if(SDL_SetTextureBlendMode(target_, SDL_BLENDMODE_BLEND))
  {
    SDL_DestroyTexture(target_);
    throw sdl::Exception();
  }
}

CanvasImpl::~CanvasImpl(void)
{
  SDL_DestroyTexture(target_);
}

void CanvasImpl::Render(SDL_Rect const* destination, double angle)
{
  if(!locked_)
  {
    if(SDL_RenderCopyEx(renderer_, target_, nullptr, destination, angle, nullptr, SDL_FLIP_NONE))
    {
      throw sdl::Exception();
    }
  }
}

void CanvasImpl::Lock(void)
{
  if(SDL_SetRenderTarget(renderer_, target_))
  {
    throw sdl::Exception();
  }
  if(SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_NONE))
  {
    throw sdl::Exception();
  }
  if(SDL_SetRenderDrawColor(renderer_, 0u, 0u, 0u, 0u))
  {
    throw sdl::Exception();
  }
  if(SDL_RenderClear(renderer_))
  {
    throw sdl::Exception();
  }
  locked_ = true;
}

void CanvasImpl::Unlock(void)
{
  if(SDL_SetRenderTarget(renderer_, nullptr))
  {
    throw sdl::Exception();
  }
  if(SDL_SetRenderDrawBlendMode(renderer_, blend_mode_))
  {
    throw sdl::Exception();
  }
  if(SDL_SetRenderDrawColor(renderer_, r_, g_, b_, a_))
  {
    throw sdl::Exception();
  }
  locked_ = false;
}

void Canvas::Lock(void)
{
  impl_->Lock();
}

void Canvas::Unlock(void)
{
  impl_->Unlock();
}

void Canvas::Render(SDL_Rect const* destination, double angle)
{
  impl_->Render(destination, angle);
}

Canvas::Canvas(void)
{
}

Canvas::Canvas(SDL_Renderer* renderer, SDL_Window* window, int w, int h) : impl_(new CanvasImpl(renderer, window, w, h))
{
}

Canvas::Canvas(Canvas const& other) : impl_(other.impl_)
{
}

Canvas::Canvas(Canvas&& other) : impl_(std::move(other.impl_))
{
}

Canvas::~Canvas(void)
{
}

Canvas& Canvas::operator=(Canvas other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}