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
#include "surface.h"
#include "hash.h"

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
  std::unordered_map<std::string, std::shared_ptr<TextureImpl>> textures_;
  typedef std::pair<std::string, std::shared_ptr<FontImpl>> TextPair;
  typedef algorithm::Hash<std::string, std::shared_ptr<FontImpl>> TextHash;
  std::unordered_map<TextPair, std::shared_ptr<TextureImpl>, TextHash> text_;
  SDL_Point view_;
  float zoom_;
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
  renderer_ = nullptr;
  window_ = nullptr;
  try
  {
    (void)SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    (void)SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    char const* name;
    int width;
    int height;
    char const* mode;

    json.Unpack("{sssisiss}",
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
      throw sdl::Exception();
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer_)
    {
      throw sdl::Exception();
    }

    if(SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE))
    {
      throw sdl::Exception();
    }

    view_ = { 0, 0 };
    zoom_ = 1.f;
  }
  catch(...)
  {
    Destroy();
    throw;
  }
}

WindowImpl::~WindowImpl(void)
{
  Destroy();
}

Texture WindowImpl::Load(std::string const& filename)
{
  Texture texture;

  auto fileiter = textures_.find(filename);
  if(fileiter != textures_.end())
  {
    texture.impl_ = fileiter->second;
  }
  else
  {
    sdl::Surface surface(filename.c_str());
    //(void)SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 255, 0));
    sdl::Texture sdl_texture(renderer_, surface);
    auto impl = std::make_shared<TextureImpl>(sdl_texture, window_, renderer_, &view_, &zoom_);
    texture.impl_ = impl;
    textures_[filename] = impl;
  }
  return texture;
}

Texture WindowImpl::Text(std::string const& text, Font const& font)
{
  Texture texture;

  auto textpair = TextPair(text, font.impl_);
  auto fileiter = text_.find(textpair);
  if(fileiter != text_.end())
  {
    texture.impl_ = fileiter->second;
  }
  else
  {
    sdl::Surface surface(font.impl_->font_, text.c_str(), font.impl_->colour_);
    sdl::Texture sdl_texture(renderer_, surface);

    auto impl = std::make_shared<TextureImpl>(sdl_texture, window_, renderer_, &view_, &zoom_);
    texture.impl_ = impl;
    text_[textpair] = impl;
  }
  return texture;
}

void WindowImpl::Clear(void) const
{
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
  auto fileiter = textures_.find(filename);
  if(fileiter != textures_.end())
  {
    textures_.erase(fileiter);
  }    
}

void WindowImpl::Free(void)
{
  textures_.clear();
}

void WindowImpl::View(int x, int y, float zoom)
{
  view_.x = x;
  view_.y = y;
  zoom_ = zoom;
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
  //typedef std::function<bool(display::BoundingBox const&, display::BoundingBox const&, float, bool, double)> TextureFunc;
  //std::weak_ptr<WindowImpl> impl = impl_;
  //TextureFunc texture;
  //texture = [impl](display::BoundingBox const& source, display::BoundingBox const& destination, float parallax, bool tile, double angle)
  //{
  //  auto iter = impl.lock();
  //  if(iter)
  //  {
  //    sdl::Render(iter->renderer_, iter->texture, source, destination, &iter->view_, iter->zoom_, parallax, tile, angle);
  //  }
  //  return bool(iter);
  //};
  //void TextureImpl::Render(SDL_Rect const* source, SDL_Rect const* destination, float parallax, bool tile, double angle) const
  //{
  //  sdl::Render(window_, renderer_, texture_, source, destination, view_, *zoom_, parallax, tile, angle);
  //}

  //void Texture::Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle) const
  //{
  //  if (auto impl = impl_.lock())
  //  {
  //    impl->Render(source, destination, parallax, tile, angle);
  //  }
  //}

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