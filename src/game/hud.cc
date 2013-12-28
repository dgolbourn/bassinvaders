#include "hud.h"
#include "bind.h"
#include <limits>
#include <mutex>

namespace game
{
class HUDImpl
{
public:
  HUDImpl(json::JSON const& json, display::Window& window);
  void Score(int score);
  void Life(int life);
  void Render(void);
  display::Window window_;
  display::Font font_;
  display::Texture score_;
  display::Texture life_;
  display::BoundingBox score_position_;
  display::BoundingBox life_position_;
  std::mutex mutex_;
};

HUDImpl::HUDImpl(json::JSON const& json, display::Window& window) : window_(window)
{
  char const* filename;
  int score_x;
  int score_y;
  int life_x;
  int life_y;
  int r, g, b, a;
  int point;

  json.Unpack("{sssis[iiii]s[ii]s[ii]}",
    "font", &filename,
    "point", &point,
    "colour", &r, &g, &b, &a,
    "score position", &score_x, &score_y,
    "life position", &life_x, &life_y);

  font_ = display::Font(std::string(filename), point , r, g, b, a);
  score_position_ = display::BoundingBox(score_x, score_y, 0, 0);
  life_position_ = display::BoundingBox(life_x, life_y, 0, 0);
  Score(0);
  Life(100);
}

void HUDImpl::Score(int score)
{
  score_ = window_.Text(std::to_string(score), font_);
}

void HUDImpl::Life(int life)
{
  life_ = window_.Text(std::to_string(life), font_);
}

void HUDImpl::Render(void)
{
  score_(display::BoundingBox(), score_position_, 0.f, false, 0.);
  life_(display::BoundingBox(), life_position_, 0.f, false, 0.);
}

void HUD::Score(int score)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Score(score);
}

void HUD::Life(int life)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Life(life);
}

HUD::operator bool(void) const
{
  return bool(impl_);
}

HUD::HUD(std::string const& filename, display::Window& window, Scene& scene) : HUD(json::JSON(filename), window, scene)
{
}

HUD::HUD(json::JSON const& json, display::Window& window, Scene& scene)
{
  impl_ = std::make_shared<HUDImpl>(json, window);
  scene.Add(event::Bind(&HUDImpl::Render, impl_), std::numeric_limits<int>().max());
}

HUD::HUD(void)
{
}

HUD::HUD(HUD const& other) : impl_(other.impl_)
{
}

HUD::HUD(HUD&& other) : impl_(std::move(other.impl_))
{
}

HUD::~HUD(void)
{
}

HUD& HUD::operator=(HUD other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}