#include "texture.h"
#include "texture_impl.h"
#include "SDL_render.h"
#include "bounding_box.h"
#include "sdl_exception.h"
#include "flood_fill.h"
#include <cmath>

namespace display
{
static inline int transform(int x, float new_origin, float half_width, float zoom, float parallax)
{
  float x0 = float(x);
  x0 -= parallax * new_origin;
  x0 -= half_width;
  x0 *= zoom;
  x0 += half_width;
  return int(x0);
}

static void RenderCopyEx(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, double angle)
{
  if(SDL_RenderCopyEx(renderer, texture, source, destination, angle, nullptr, SDL_FLIP_NONE))
  {
    throw sdl::Exception();
  }
}

static void EnclosingRect(SDL_Rect const& rect, double angle, SDL_Rect& result)
{
  float w = .5f * float(rect.w);
  float h = .5f * float(rect.h);
  float c = float(std::cos(angle));
  float s = float(std::sin(angle));
  float cw = c * w;
  float ch = c * h;
  float sw = s * w;
  float sh = s * h;

  SDL_Point points[4];
  points[0].x = int(-cw + sh);
  points[0].y = int(ch + sw);
  points[1].x = int(cw + sh);
  points[1].y = int(ch - sw);
  points[2].x = int(-cw - sh);
  points[2].y = int(-ch + sw);
  points[3].x = int(cw - sh);
  points[3].y = int(-ch - sw);

  (void)SDL_EnclosePoints(points, 4, nullptr, &result);
  result.x += rect.x + int(w);
  result.y += rect.y + int(h);
}

class Painter
{
private:
  float north_[2];
  float east_[2];
  SDL_Rect painting_region_;
  SDL_Rect collision_box_;
  int offset_[2];

  SDL_Renderer* renderer_; 
  SDL_Window* window_; 
  SDL_Texture* texture_;
  SDL_Rect const* source_;
  SDL_Rect destination_;
  double angle_;
public:
  Painter(SDL_Renderer* renderer, SDL_Window* window, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, View const& view, double angle)
  {
    renderer_ = renderer;
    window_ = window;
    texture_ = texture;
    source_ = source;
    destination_ = *destination;
    painting_region_ = {0, 0, view.width_, view.height_};
    angle_ = angle;
    offset_[0] = 0;
    offset_[1] = 0;
    angle *= M_PI / 180.f;
    float c = std::cos(float(angle));
    float s = std::sin(float(angle));
    east_[0] = c * float(destination_.w);
    east_[1] = s * float(destination_.w);
    north_[0] = -s * float(destination_.h);
    north_[1] = c * float(destination_.h);
   
    EnclosingRect(destination_, angle, collision_box_);

    float a = c * float(destination_.x) + s * float(destination_.y);
    a /= float(destination_.w);
    a = std::round(a);

    float b = -s * float(destination_.x) + c * float(destination_.y);
    b /= float(destination_.h);
    b = std::round(b);

    for(int i = 1; i >= -1; --i)
    {
      bool break_flag = false;
      for(int j = 1; j >= -1; --j)
      {
        float fj = float(j);
        float fi = float(i);
        float adjust[2];
        adjust[0] = (a + fi) * east_[0] + (b + fj) * north_[0];
        adjust[1] = (a + fi) * east_[1] + (b + fj) * north_[1];

        SDL_Rect temp = collision_box_;
        temp.x -= int(adjust[0]);
        temp.y -= int(adjust[1]);

        if(SDL_TRUE == SDL_HasIntersection(&temp, &painting_region_))
        {
          destination_.x -= int(adjust[0]);
          destination_.y -= int(adjust[1]);
          collision_box_ = temp;
          break_flag = true;
          break;
        }
      }
      if(break_flag)
      {
        break;
      }
    }
  }

  bool Fill(algorithm::NodeCoordinates const& coords)
  {
    bool filled = false;
    float east_offset = float(coords.first);
    float north_offset = float(coords.second);
    SDL_Point offset;
    offset.x = int(east_[0] * east_offset + north_[0] * north_offset);
    offset.y = int(east_[1] * east_offset + north_[1] * north_offset);
    collision_box_.x += offset.x;
    collision_box_.y += offset.y;
    if(SDL_TRUE == SDL_HasIntersection(&collision_box_, &painting_region_))
    {
      destination_.x += offset.x;
      destination_.y += offset.y;
      RenderCopyEx(renderer_, texture_, source_, &destination_, angle_);
      destination_.x -= offset.x;
      destination_.y -= offset.y;
      filled = true;
    }
    collision_box_.x -= offset.x;
    collision_box_.y -= offset.y;
    return filled;
  }
};

static void RenderTile(SDL_Renderer* renderer, SDL_Window* window, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect* destination, View const& view, double angle)
{
  angle = 45.;
  static float adj = 0.1f;
  adj += 0.1f;
  angle += adj;
  algorithm::FloodFill<Painter>(Painter(renderer, window, texture, source, destination, view, angle));
  //sdl::Canvas canvas;
  //if(angle)
  //{
  //  canvas = sdl::Canvas(renderer, window, view.width_, view.height_);
  //  canvas.Lock();
  //}

  //int const width = abs(destination->w);
  //int const height = abs(destination->h);
  //destination->x %= width;
  //destination->x -= width;
  //destination->y %= height;
  //destination->y -= height;
  //int const ystart = destination->y;
  //while(destination->x < view.width_)
  //{
  //  destination->y = ystart;
  //  while(destination->y < view.height_)
  //  {
  //    RenderCopyEx(renderer, texture, source, destination, 0.);
  //    destination->y += height;
  //  }
  //  destination->x += width;
  //}

  //if(angle)
  //{
  //  canvas.Unlock();
  //  SDL_Rect rect = { 0, 0, view.width_, view.height_ };
  //  canvas.Render(&rect, angle, nullptr);
  //}
}

static void RenderCopy(SDL_Renderer* renderer, SDL_Window* window, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, View const& view, float parallax, bool tile, double angle)
{
  if(destination)
  {
    if(parallax > 0.f)
    {
      SDL_Rect adjusted;
      adjusted.x = transform(destination->x, view.x_, view.half_width_, view.zoom_, parallax);
      adjusted.y = transform(destination->y, view.y_, view.half_height_, view.zoom_, parallax);
      adjusted.w = transform(destination->w, 0.f, 0.f, view.zoom_, 0.f);
      adjusted.h = transform(destination->h, 0.f, 0.f, view.zoom_, 0.f);
      if(tile)
      {
        RenderTile(renderer, window, texture, source, &adjusted, view, angle);
      }
      else
      {
        RenderCopyEx(renderer, texture, source, &adjusted, angle);
      }
    }
    else
    { 
      if(tile)
      {
        SDL_Rect adjusted = *destination;
        RenderTile(renderer, window, texture, source, &adjusted, view, angle);
      }
      else
      {
        RenderCopyEx(renderer, texture, source, destination, angle);
      }
    }
  }
  else
  {
    RenderCopyEx(renderer, texture, source, nullptr, angle);
  }
}

void TextureImpl::Render(void) const
{
  RenderCopy(renderer_, window_, texture_, nullptr, nullptr, View(), 0.f, false, 0.);
}

void TextureImpl::Render(SDL_Rect const* source, SDL_Rect const* destination, float parallax, bool tile, double angle) const
{
  RenderCopy(renderer_, window_, texture_, source, destination, view_, parallax, tile, angle);
}

Texture::Texture(void)
{
}

TextureImpl::TextureImpl(SDL_Texture* texture, SDL_Renderer* renderer, SDL_Window* window, View& view) : texture_(texture), window_(window), renderer_(renderer), view_(view)
{
}

TextureImpl::~TextureImpl(void)
{
  SDL_DestroyTexture(texture_);
}

Texture::Texture(Texture const& other) : impl_(other.impl_)
{
}

Texture::Texture(Texture&& other) : impl_(std::move(other.impl_))
{
}

Texture::~Texture(void)
{
}

Texture& Texture::operator=(Texture other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

void Texture::Render(void) const
{
  impl_->Render();
}

void Texture::Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle) const
{
  impl_->Render(source, destination, parallax, tile, angle);
}
}