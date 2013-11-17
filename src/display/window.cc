#include "window.h"
#include <unordered_map>
#include "sdl_manager.h"
#include "img_manager.h"
#include "ttf_manager.h"
#include "sdl_exception.h"
#include "img_exception.h"
#include "ttf_exception.h"
#include "texture_impl.h"
#include "font_impl.h"

namespace display
{
class WindowImpl
{
public:
  WindowImpl(std::string const& name);
  Texture Load(std::string const& filename);
  Texture Text(std::string const& text, Font const& font);
  void Free(std::string const& filename);
  void Free(void);
  void Clear(void) const;
  void Show(void) const;
  void Destroy(void);

  ~WindowImpl(void);

  SDL_Window* window_;
  SDL_Renderer* renderer_;
  std::unordered_map<std::string, Texture> files_;
  sdl::Library const sdl_;
  img::Library const img_;
  ttf::Library const ttf_;
};

void WindowImpl::Destroy(void)
{
  if(renderer_)
  {
    SDL_DestroyRenderer(renderer_);
  }
  if(window_)
  {
    SDL_DestroyWindow(window_);
  }
}

WindowImpl::WindowImpl(std::string const& name) : sdl_(SDL_INIT_VIDEO), img_(IMG_INIT_PNG), ttf_()
{
  (void)SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
  (void)SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  window_ = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, 
    SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
  if(!window_)
  {
    Destroy();
    throw sdl::Exception();
  }

  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if(!renderer_)
  {
    Destroy();
    throw sdl::Exception();
  }

  if(SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF))
  {
    Destroy();
    throw sdl::Exception();
  }
}

WindowImpl::~WindowImpl(void)
{
  Destroy();
}

Texture WindowImpl::Load(std::string const& filename)
{
  Texture texture;

  auto fileiter = files_.find(filename);
  if(fileiter != files_.end())
  {
    texture = fileiter->second;
  }
  else
  {
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if(!surface)
    {
      throw img::Exception();
    }
    (void)SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 255, 0));

    SDL_Texture* sdl_texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_FreeSurface(surface);
    if(!sdl_texture)
    { 
      throw sdl::Exception();
    }

    texture.impl_ = std::shared_ptr<TextureImpl>(new TextureImpl(sdl_texture, renderer_));
    files_[filename] = texture;
  }

  return texture;
}

Texture WindowImpl::Text(std::string const& text, Font const& font)
{
  SDL_Surface* surface = TTF_RenderText_Solid(font.impl_->font_, text.c_str(),
    font.impl_->colour_);
  if(!surface)
  {
    throw ttf::Exception();
  }

  SDL_Texture* sdl_texture = SDL_CreateTextureFromSurface(renderer_, surface);
  SDL_FreeSurface(surface);
  if(!sdl_texture)
  {
    throw sdl::Exception();
  }

  Texture texture;
  texture.impl_ = std::shared_ptr<TextureImpl>(new TextureImpl(sdl_texture, renderer_));

  return texture;
}

void WindowImpl::Clear(void) const
{
  if(SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF))
  {
    throw sdl::Exception();
  }
  if(SDL_RenderClear(renderer_))
  {
    throw sdl::Exception();
  }
}

void WindowImpl::Show(void) const
{
  SDL_RenderPresent(renderer_);
}

void WindowImpl::Free(std::string const& filename)
{
  auto fileiter = files_.find(filename);
  if(fileiter != files_.end())
  {
    files_.erase(fileiter);
  }    
}

void WindowImpl::Free(void)
{
  files_.clear();
}

Window::Window(std::string const& name) : impl_(new WindowImpl(name))
{
}

Window::Window(void)
{
}

Window::Window(Window const& other) : impl_(other.impl_)
{
}

Window::Window(Window&& other) : impl_(std::move(other.impl_))
{
}

Window::~Window(void)
{
}

Window& Window::operator=(Window other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

Texture Window::Load(std::string const& filename)
{
  return impl_->Load(filename);
}

void Window::Clear(void) const
{
  return impl_->Clear();
}

void Window::Show(void) const
{
  return impl_->Show();
}

Texture Window::Text(std::string const& text, Font const& font)
{
  return impl_->Text(text, font);
}

void Window::Free(std::string const& filename)
{
  impl_->Free(filename);
}

void Window::Free(void)
{
  impl_->Free();
}
}