#include "bounding_box.h"
#include "SDL_rect.h"
#include "rect.h"

namespace display
{
class BoundingBoxImpl
{
public:
  BoundingBoxImpl(int x, int y, int w, int h);
  BoundingBoxImpl(json::JSON const& json);
  SDL_Rect rect_;
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

BoundingBox::operator SDL_Rect*(void) const
{
  SDL_Rect* rect = nullptr;
  if(impl_)
  {
    rect = &impl_->rect_;
  }
  return rect;
}

void BoundingBox::x(int x)
{
  impl_->rect_.x = x;
}

void BoundingBox::y(int y)
{
  impl_->rect_.y = y;
}

void BoundingBox::w(int w)
{
  impl_->rect_.w = w;
}

void BoundingBox::h(int h)
{
  impl_->rect_.h = h;
}

int BoundingBox::x(void) const
{
  return impl_->rect_.x;
}

int BoundingBox::y(void) const
{
  return impl_->rect_.y;
}

int BoundingBox::w(void) const
{
  return impl_->rect_.w;
}

int BoundingBox::h(void) const
{
  return impl_->rect_.h;
}

BoundingBox BoundingBox::Copy(void) const
{
  return BoundingBox(impl_->rect_.x, impl_->rect_.y, impl_->rect_.w, impl_->rect_.h);
}

void BoundingBox::Copy(BoundingBox const& other)
{
  impl_->rect_ = other.impl_->rect_;
}

BoundingBox::BoundingBox(void)
{
}

BoundingBox::BoundingBox(int x, int y, int w, int h)
{
  impl_ = std::make_shared<BoundingBoxImpl>(x, y, w, h);
}

BoundingBoxImpl::BoundingBoxImpl(int x, int y, int w, int h) : rect_({x, y, w, h})
{
}

BoundingBoxImpl::BoundingBoxImpl(json::JSON const& json)
{
  json.Unpack("[iiii]", &rect_.x, &rect_.y, &rect_.w, &rect_.h);
}

BoundingBox::BoundingBox(std::string const& filename) : BoundingBox(json::JSON(filename))
{
}

BoundingBox::BoundingBox(json::JSON const& json)
{
  impl_ = std::make_shared<BoundingBoxImpl>(json);
}

BoundingBox::BoundingBox(BoundingBox const& other) : impl_(other.impl_)
{
}

BoundingBox::BoundingBox(BoundingBox&& other) : impl_(std::move(other.impl_))
{
}

BoundingBox::~BoundingBox(void)
{
}

BoundingBox& BoundingBox::operator=(BoundingBox other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}