#include "window.h"
#include <memory>
#include <map>
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
  WindowImpl(std::string name);
  ~WindowImpl(void);

  Texture Load(std::string filename);
  Texture Text(std::string text, Font font);
  void Free(std::string filename);
  void Clear(void);
  void Show(void);

  SDL_Window* window_;
  SDL_Renderer* renderer_;
  std::map<std::string, Texture> files_;
};

WindowImpl::WindowImpl(std::string name)
{
  sdl::Init(SDL_INIT_VIDEO);
  img::Init(IMG_INIT_PNG);
  ttf::Init();

  SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  window_ = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, 
    SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);

  if(!window_)
  {
    throw sdl::Exception();
  }
  else
  {
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer_)
    {
      throw sdl::Exception();
    }
    else
    {
      SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);
    }
  }
}

WindowImpl::~WindowImpl(void)
{
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
  
  ttf::Quit();
  img::Quit(IMG_INIT_PNG);
  sdl::Quit(SDL_INIT_VIDEO);
}

Texture WindowImpl::Load(std::string filename)
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
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 255, 0));

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

Texture WindowImpl::Text(std::string text, Font font)
{
  SDL_Surface* surface = TTF_RenderText_Solid(font.impl_->font_, text.c_str(),
    *font.impl_->colour_);
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

void WindowImpl::Clear(void)
{
  SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF );
  SDL_RenderClear(renderer_);
}

void WindowImpl::Show(void)
{
  SDL_RenderPresent(renderer_);
}

void WindowImpl::Free(std::string filename)
{
  auto fileiter = files_.find(filename);
  if(fileiter != files_.end())
  {
    files_.erase(fileiter);
  }    
}

Window::Window(std::string name)
{
  impl_ = std::shared_ptr<class WindowImpl>(new WindowImpl(name));
}

Window::Window(void)
{
}

Window::Window(const Window& original)
{
  impl_ = original.impl_;
}

Window::Window(Window&& original)
{
  impl_ = original.impl_;
  original.impl_.reset();
}

Window::~Window(void)
{
}

Window& Window::operator=(Window original)
{
  std::swap(impl_, original.impl_);
  return *this;
}

Texture Window::Load(std::string filename)
{
  return impl_->Load(filename);
}

void Window::Clear(void)
{
  return impl_->Clear();
}

void Window::Show(void)
{
  return impl_->Show();
}

Texture Window::Text(std::string text, Font font)
{
  return impl_->Text(text, font);
}

void Window::Free(std::string filename)
{
  impl_->Free(filename);
}

}