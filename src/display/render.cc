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
  Painter(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, SDL_Point const* view, double angle);
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
  float w = .5f * w_;
  float h = .5f * h_;
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
  float x = float(destination_.x);
  float y = float(destination_.y);
  float a = std::round((c_ * x + s_ * y) / w_);
  float b = std::round((-s_ * x + c_ * y) / h_);

  float sign[] = { 1.f, 0.f, -1.f };
  for(float const& i : sign)
  {
    bool break_flag = false;
    for(float const& j : sign)
    {
      SDL_Point adjust;
      adjust.x = int((a + i) * east_[0] + (b + j) * north_[0]);
      adjust.y = int((a + i) * east_[1] + (b + j) * north_[1]);

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

Painter::Painter(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, SDL_Point const* view, double angle)
{
  renderer_ = renderer;
  texture_ = texture;
  source_ = source;
  destination_ = *destination;
  angle_ = angle;
  clip_.x = view->x;
  clip_.y = view->y;
  SDL_GetWindowSize(window, &clip_.w, &clip_.h);
 
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
  SDL_Rect collision_box = collision_box_;
  collision_box.x += move.x;
  collision_box.y += move.y;
  if(SDL_TRUE == SDL_HasIntersection(&collision_box, &clip_))
  {
    SDL_Rect destination = destination_;
    destination.x += move.x;
    destination.y += move.y;
    RenderCopy(renderer_, texture_, source_, &destination, angle_);
    filled = true;
  }
  return filled;
}

static int Transform(int x, int new_origin, int width, float zoom, float parallax)
{
  float x0 = float(x);
  float w = .5f * float(width);
  x0 -= parallax * float(new_origin);
  x0 -= w;
  x0 *= zoom;
  x0 += w;
  return int(x0);
}

void Render(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_Rect const* destination, SDL_Point const* view, float zoom, float parallax, bool tile, double angle)
{
  if(destination)
  {
    SDL_Rect adjusted;
    if(parallax > 0.f)
    {
      int w;
      int h;
      SDL_GetWindowSize(window, &w, &h);
      adjusted.x = Transform(destination->x, view->x, w, zoom, parallax);
      adjusted.y = Transform(destination->y, view->y, h, zoom, parallax);
      adjusted.w = int(zoom * float(destination->w));
      adjusted.h = int(zoom * float(destination->h));
    }
    else
    {
      adjusted = *destination;
    }
   
    if(tile)
    {
      SDL_Point tile_view = {0, 0};
      algorithm::FloodFill<Painter>()(Painter(window, renderer, texture, source, &adjusted, &tile_view, angle));
    }
    else
    {
      RenderCopy(renderer, texture, source, &adjusted, angle);
    }
  }
  else
  {
    RenderCopy(renderer, texture, source, nullptr, angle);
  }
}
}