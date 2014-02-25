#include "bounding_box.h"
#include "SDL_sysrender.h"
#include "rect.h"

namespace display
{
class BoundingBoxImpl
{
public:
  BoundingBoxImpl(float x, float y, float w, float h);
  BoundingBoxImpl(json::JSON const& json);
  SDL_FRect rect_;
};

bool BoundingBox::operator&&(BoundingBox const& other) const
{
  return sdl::Intersection(&impl_->rect_, &other.impl_->rect_);
}

bool BoundingBox::operator<(BoundingBox const& other) const
{
  return impl_.owner_before(other.impl_);
}

BoundingBox::operator bool(void) const
{
  return bool(impl_);
}

void BoundingBox::x(float x)
{
  impl_->rect_.x = x;
}

void BoundingBox::y(float y)
{
  impl_->rect_.y = y;
}

void BoundingBox::w(float w)
{
  impl_->rect_.w = w;
}

void BoundingBox::h(float h)
{
  impl_->rect_.h = h;
}

float BoundingBox::x(void) const
{
  return impl_->rect_.x;
}

float BoundingBox::y(void) const
{
  return impl_->rect_.y;
}

float BoundingBox::w(void) const
{
  return impl_->rect_.w;
}

float BoundingBox::h(void) const
{
  return impl_->rect_.h;
}

BoundingBox BoundingBox::Copy(void) const
{
  return BoundingBox(impl_->rect_.x, impl_->rect_.y, impl_->rect_.w, impl_->rect_.h);
}

void BoundingBox::Copy(BoundingBox const& other) const
{
  impl_->rect_ = other.impl_->rect_;
}

BoundingBox::BoundingBox(float x, float y, float w, float h)
{
  impl_ = std::make_shared<BoundingBoxImpl>(x, y, w, h);
}

BoundingBoxImpl::BoundingBoxImpl(float x, float y, float w, float h) : rect_({x, y, w, h})
{
}

BoundingBoxImpl::BoundingBoxImpl(json::JSON const& json)
{
  int x, y, w, h;
  json.Unpack("[iiii]", &x, &y, &w, &h);
  rect_.x = (float)x;
  rect_.y = (float)y;
  rect_.w = (float)w;
  rect_.h = (float)h;
}

BoundingBox::BoundingBox(json::JSON const& json)
{
  impl_ = std::make_shared<BoundingBoxImpl>(json);
}
}