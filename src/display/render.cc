#include "render.h"
#include "sdl_exception.h"
#include "flood_fill.h"
#include <cmath>

namespace sdl
{
static void RenderCopy(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, double angle)
{
  if(SDL_RenderCopyEx(renderer, texture, source, destination, angle, nullptr, SDL_FLIP_NONE))
  {
    throw sdl::Exception();
  }
}

class Painter
{
public:
  Painter(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, View const& view, double angle);
  bool Fill(algorithm::NodeCoordinates const& coords);
  void MovementVectors(void);
  void EnclosingRect(void);
  void StartingRect(void);
  
  float north_[2];
  float east_[2];
  SDL_Rect clip_;
  SDL_Rect collision_box_;
  SDL_Renderer* renderer_;
  SDL_Texture* texture_;
  SDL_Rect const* source_;
  SDL_Rect destination_;
  double angle_;
  float c_;
  float s_;
  float w_;
  float h_;
};

void Painter::EnclosingRect(void)
{
  float w = .5f * float(destination_.w);
  float h = .5f * float(destination_.h);
  float cw = c_ * w;
  float ch = c_ * h;
  float sw = s_ * w;
  float sh = s_ * h;

  SDL_Point points[4];
  points[0].x = int(-cw + sh);
  points[0].y = int(ch + sw);
  points[1].x = int(cw + sh);
  points[1].y = int(ch - sw);
  points[2].x = int(-cw - sh);
  points[2].y = int(-ch + sw);
  points[3].x = int(cw - sh);
  points[3].y = int(-ch - sw);

  (void)SDL_EnclosePoints(points, 4, nullptr, &collision_box_);
  collision_box_.x += destination_.x + int(w);
  collision_box_.y += destination_.y + int(h);
}

void Painter::StartingRect(void)
{
  float fx = float(destination_.x);
  float fy = float(destination_.y);
  float a = std::round((c_ * fx + s_ * fy) / w_);
  float b = std::round((-s_ * fx + c_ * fy) / h_);

  float idx[] = { 1, 0, -1 };
  for(float const& fi : idx)
  {
    bool break_flag = false;
    for(float const& fj : idx)
    {
      SDL_Point adjust;
      adjust.x = int((a + fi) * east_[0] + (b + fj) * north_[0]);
      adjust.y = int((a + fi) * east_[1] + (b + fj) * north_[1]);

      SDL_Rect temp = collision_box_;
      temp.x -= adjust.x;
      temp.y -= adjust.y;

      if(SDL_TRUE == SDL_HasIntersection(&temp, &clip_))
      {
        collision_box_ = temp;
        destination_.x -= adjust.x;
        destination_.y -= adjust.y;
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

void Painter::MovementVectors(void)
{
  float angle = float(angle_ * M_PI / 180.);
  c_ = std::cos(angle);
  s_ = std::sin(angle);
  w_ = float(destination_.w);
  h_ = float(destination_.h);
  east_[0] = c_ * w_;
  east_[1] = s_ * w_;
  north_[0] = -s_ * h_;
  north_[1] = c_ * h_;
}

Painter::Painter(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, View const& view, double angle)
{
  renderer_ = renderer;
  texture_ = texture;
  source_ = source;
  destination_ = *destination;
  angle_ = angle;
  clip_ = { 0, 0, view.width_, view.height_ };
 
  MovementVectors();
  EnclosingRect();
  StartingRect();
}

bool Painter::Fill(algorithm::NodeCoordinates const& coords)
{
  bool filled = false;
  float east = float(coords.first);
  float north = float(coords.second);
  SDL_Point move;
  move.x = int(east_[0] * east + north_[0] * north);
  move.y = int(east_[1] * east + north_[1] * north);
  collision_box_.x += move.x;
  collision_box_.y += move.y;
  if(SDL_TRUE == SDL_HasIntersection(&collision_box_, &clip_))
  {
    destination_.x += move.x;
    destination_.y += move.y;
    RenderCopy(renderer_, texture_, source_, &destination_, angle_);
    destination_.x -= move.x;
    destination_.y -= move.y;
    filled = true;
  }
  collision_box_.x -= move.x;
  collision_box_.y -= move.y;
  return filled;
}

static void RenderTile(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect* destination, View const& view, double angle)
{
  algorithm::FloodFill<Painter>(Painter(renderer, texture, source, destination, view, angle));
}

static int Transform(int x, float new_origin, float half_width, float zoom, float parallax)
{
  float x0 = float(x);
  x0 -= parallax * new_origin;
  x0 -= half_width;
  x0 *= zoom;
  x0 += half_width;
  return int(x0);
}

void Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, View const& view, float parallax, bool tile, double angle)
{
  if(destination)
  {
    if(parallax > 0.f)
    {
      SDL_Rect adjusted;
      adjusted.x = Transform(destination->x, view.x_, view.half_width_, view.zoom_, parallax);
      adjusted.y = Transform(destination->y, view.y_, view.half_height_, view.zoom_, parallax);
      adjusted.w = Transform(destination->w, 0.f, 0.f, view.zoom_, 0.f);
      adjusted.h = Transform(destination->h, 0.f, 0.f, view.zoom_, 0.f);
      if(tile)
      {
        RenderTile(renderer, texture, source, &adjusted, view, angle);
      }
      else
      {
        RenderCopy(renderer, texture, source, &adjusted, angle);
      }
    }
    else
    { 
      if(tile)
      {
        SDL_Rect adjusted = *destination;
        RenderTile(renderer, texture, source, &adjusted, view, angle);
      }
      else
      {
        RenderCopy(renderer, texture, source, destination, angle);
      }
    }
  }
  else
  {
    RenderCopy(renderer, texture, source, nullptr, angle);
  }
}
}