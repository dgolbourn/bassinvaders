#include "window.h"
#include <unordered_map>
#include "SDL_image.h"
#include "sdl_library.h"
#include "img_library.h"
#include "ttf_library.h"
#include "sdl_exception.h"
#include "texture.h"
#include "font_impl.h"
#include "surface.h"
#include "hash.h"
#include "render.h"
#include "thread.h"

namespace display
{
class WindowImpl
{
public:
  WindowImpl(json::JSON const& json);
  sdl::Texture::WeakPtr Load(std::string const& filename);
  sdl::Texture Text(std::string const& text, Font const& font);
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
  std::unordered_map<std::string, sdl::Texture> textures_;
  SDL_Point view_;
  float zoom_;
  std::mutex mutex_;
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

sdl::Texture::WeakPtr WindowImpl::Load(std::string const& filename)
{
  sdl::Texture::WeakPtr texture_ptr;
  auto fileiter = textures_.find(filename);
  if(fileiter != textures_.end())
  {
    texture_ptr = fileiter->second;
  }
  else
  {
    sdl::Texture texture(renderer_, sdl::Surface(filename.c_str()));
    textures_[filename] = texture;
    texture_ptr = texture;
  }
  return texture_ptr;
}

sdl::Texture WindowImpl::Text(std::string const& text, Font const& font)
{
  sdl::Surface surface(font.impl_->font_, text.c_str(), font.impl_->colour_);
  SDL_SetSurfaceAlphaMod(surface, font.impl_->colour_.a);
  return sdl::Texture(renderer_, surface);
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

void WindowImpl::Free(void)
{
  textures_.clear();
}

void WindowImpl::View(int x, int y, float zoom)
{
  int w;
  int h;
  SDL_GetWindowSize(window_, &w, &h);
  view_.x = x - w/2;
  view_.y = y - h/2;
  zoom_ = zoom;
}

Window::Window(json::JSON const& json)
{
  impl_ = std::make_shared<WindowImpl>(json);
}

Window::Window(void)
{
}

static void Render(std::shared_ptr<WindowImpl> const& window, sdl::Texture const& texture, BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle)
{
  SDL_Rect* source_ptr = nullptr;
  SDL_Rect source_copy;
  if(source)
  {
    source_copy = source;
    source_ptr = &source_copy;
  }

  SDL_Rect* destination_ptr = nullptr;
  SDL_Rect destination_copy;
  if(destination)
  {
    destination_copy = destination;
    destination_ptr = &destination_copy;
  }

  thread::Lock(window->mutex_);
  sdl::Render(window->window_, window->renderer_, texture, source_ptr, destination_ptr, &window->view_, window->zoom_, parallax, tile, angle);
}

static Texture Bind(std::weak_ptr<WindowImpl> window_ptr, sdl::Texture::WeakPtr texture_ptr)
{
  return [window_ptr, texture_ptr](BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle)
  {
    bool locked = false;
    if(auto window = window_ptr.lock())
    {
      if(auto texture = texture_ptr.Lock())
      {
        Render(window, texture, source, destination, parallax, tile, angle);
        locked = true;
      }
    }
    return locked;
  };
}

static Texture Bind(std::weak_ptr<WindowImpl> window_ptr, sdl::Texture texture)
{
  return [window_ptr, texture](BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle)
  {
    bool locked = false;
    if(auto window = window_ptr.lock())
    {
      Render(window, texture, source, destination, parallax, tile, angle);
      locked = true;
    }
    return locked;
  };
}

Texture Window::Load(std::string const& filename)
{
  thread::Lock(impl_->mutex_);
  return Bind(impl_, impl_->Load(filename));
}

Texture Window::Text(std::string const& text, Font const& font)
{
  thread::Lock(impl_->mutex_);
  return Bind(impl_, impl_->Text(text, font));
}

void Window::Clear(void) const
{
  thread::Lock(impl_->mutex_);
  return impl_->Clear();
}

void Window::Show(void) const
{
  thread::Lock(impl_->mutex_);
  return impl_->Show();
}

void Window::Free(void)
{
  thread::Lock(impl_->mutex_);
  impl_->Free();
}

void Window::View(int x, int y, float zoom)
{
  thread::Lock(impl_->mutex_);
  impl_->View(x, y, zoom);
}
}