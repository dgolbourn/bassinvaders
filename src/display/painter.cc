#include "painter.h"
#include "render.h"
#include "rect.h"
#include <cmath>
namespace sdl
{
class PainterImpl
{
public:
  PainterImpl(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, double angle);
  bool operator()(algorithm::NodeCoordinates const& coords);
  void MovementVectors(void);
  void EnclosingRect(void);
  void StartingRect(void);

  std::pair<float, float> north_;
  std::pair<float, float> east_;
  SDL_FRect clip_;
  SDL_FRect collision_box_;
  SDL_Renderer* renderer_;
  SDL_Texture* texture_;
  SDL_Rect const* source_;
  SDL_FRect destination_;
  double angle_;
  float c_;
  float s_;
  float w_;
  float h_;
};

void PainterImpl::EnclosingRect(void)
{
  float w = .5f * w_;
  float h = .5f * h_;
  float cw = c_ * w;
  float ch = c_ * h;
  float sw = s_ * w;
  float sh = s_ * h;

  static int const size = 4;
  SDL_FPoint points[size];
  points[0].x = -cw + sh;
  points[0].y = ch + sw;
  points[1].x = cw + sh;
  points[1].y = ch - sw;
  points[2].x = -cw - sh;
  points[2].y = -ch + sw;
  points[3].x = cw - sh;
  points[3].y = -ch - sw;

  EnclosePoints(points, size, &collision_box_);
  collision_box_.x += destination_.x + w;
  collision_box_.y += destination_.y + h;
}

void PainterImpl::StartingRect(void)
{
  float a = std::round((c_ * destination_.x + s_ * destination_.y) / w_);
  float b = std::round((-s_ * destination_.x + c_ * destination_.y) / h_);

  static float const sign[] = {1.f, 0.f, -1.f};
  for(float i : sign)
  {
    bool break_flag = false;
    for(float j : sign)
    {
      SDL_FPoint adjust;
      adjust.x = (a + i) * east_.first + (b + j) * north_.first;
      adjust.y = (a + i) * east_.second + (b + j) * north_.second;

      SDL_FRect temp = collision_box_;
      temp.x -= adjust.x;
      temp.y -= adjust.y;

      if(sdl::Intersection(&temp, &clip_))
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

void PainterImpl::MovementVectors(void)
{
  double angle = angle_ * M_PI / 180.;
  c_ = (float)std::cos(angle);
  s_ = (float)std::sin(angle);
  w_ = destination_.w;
  h_ = destination_.h;
  east_.first = c_ * w_;
  east_.second = s_ * w_;
  north_.first = -s_ * h_;
  north_.second = c_ * h_;
}

PainterImpl::PainterImpl(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, double angle)
{
  renderer_ = renderer;
  texture_ = texture;
  source_ = source;
  destination_ = *destination;
  angle_ = angle;
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  clip_.w = (float)w;
  clip_.h = (float)h;
  clip_.x = 0.f;
  clip_.y = 0.f;

  MovementVectors();
  EnclosingRect();
  StartingRect();
}

bool PainterImpl::operator()(algorithm::NodeCoordinates const& coords)
{
  bool filled = false;
  float east = float(coords.first);
  float north = float(coords.second);
  SDL_FPoint move;
  move.x = east_.first * east + north_.first * north;
  move.y = east_.second * east + north_.second * north;
  SDL_FRect collision_box = collision_box_;
  collision_box.x += move.x;
  collision_box.y += move.y;
  if(sdl::Intersection(&collision_box, &clip_))
  {
    SDL_FRect destination = destination_;
    destination.x += move.x;
    destination.y += move.y;
    Render(renderer_, texture_, source_, &destination, angle_);
    filled = true;
  }
  return filled;
}

bool Painter::operator()(algorithm::NodeCoordinates const& coords)
{
  return (*impl_)(coords);
}

Painter::Painter(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, double angle)
{
  impl_ = std::make_shared<PainterImpl>(window, renderer, texture, source, destination, angle);
}
}