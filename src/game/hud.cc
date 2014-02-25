#include "hud.h"
#include "bind.h"
#include <limits>

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

  font_ = display::Font(std::string(filename), point, r, g, b, a);
  score_position_ = display::BoundingBox((float)score_x, (float)score_y, 0.f, 0.f);
  life_position_ = display::BoundingBox((float)life_x, (float)life_y, 0.f, 0.f);
  Score(0);
  Life(0);
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
  impl_->Score(score);
}

void HUD::Life(int life)
{
  impl_->Life(life);
}

HUD::operator bool(void) const
{
  return bool(impl_);
}

HUD::HUD(json::JSON const& json, display::Window& window, Scene& scene)
{
  impl_ = std::make_shared<HUDImpl>(json, window);
  scene.Add(event::Bind(&HUDImpl::Render, impl_), std::numeric_limits<int>().max());
}
}