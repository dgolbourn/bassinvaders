#include "window.h"
#include <unordered_map>
#include "sdl_library.h"
#include "img_library.h"
#include "ttf_library.h"
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
  WindowImpl(json::JSON const& json);
  Texture Load(std::string const& filename);
  Texture Text(std::string const& text, Font const& font);
  void Free(std::string const& filename);
  void Free(void);
  void Clear(void) const;
  void Show(void) const;
  void Destroy(void);
  void View(int x, int y, float zoom);

  ~WindowImpl(void);

  sdl::Library const sdl_;
  img::Library const img_;
  ttf::Library const ttf_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;
  std::unordered_map<std::string, Texture> files_;
  display::View view_;
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

WindowImpl::WindowImpl(json::JSON const& json) : sdl_(SDL_INIT_VIDEO), img_(IMG_INIT_PNG), ttf_()
{
  (void)SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
  (void)SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  char const* name;
  int width;
  int height;
  char const* mode;

  json.Unpack("{sssisiss}", 0,
    "name", &name,
    "width", &width,
    "height", &height,
    "mode", &mode);

  Uint32 flags = 0;
  if(!strcmp(mode, "Fullscreen"))
  {
    flags |= SDL_WINDOW_FULLSCREEN;
  }
  else if(!strcmp(mode, "Borderless"))
  {
    flags |= SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED;
  }

  window_ = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
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

  view_ = {0.f, 0.f, 0.5f*float(width), 0.5f*float(height), 1.f, width, height};
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

    texture.impl_ = std::make_shared<TextureImpl>(sdl_texture, renderer_, view_);
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
  texture.impl_ = std::make_shared<TextureImpl>(sdl_texture, renderer_, view_);
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

void WindowImpl::View(int x, int y, float zoom)
{
  view_.x_ = float(x);
  view_.y_ = float(y);
  view_.zoom_ = zoom;
}

Window::Window(std::string const& filename) : impl_(new WindowImpl(json::JSON(filename)))
{
}

Window::Window(json::JSON const& json) : impl_(new WindowImpl(json))
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

void Window::View(int x, int y, float zoom)
{
  impl_->View(x, y, zoom);
}
}