#include "bounding_box.h"
#include "SDL_rect.h"

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
  return SDL_HasIntersection(&impl_->rect_, &other.impl_->rect_) != SDL_FALSE;
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

int& BoundingBox::x(void) const
{
  return impl_->rect_.x;
}

int& BoundingBox::y(void) const
{
  return impl_->rect_.y;
}

int& BoundingBox::w(void) const
{
  return impl_->rect_.w;
}

int& BoundingBox::h(void) const
{
  return impl_->rect_.h;
}

BoundingBox::BoundingBox(void)
{
}

BoundingBox::BoundingBox(int x, int y, int w, int h) : impl_(new BoundingBoxImpl(x, y, w, h))
{
}

BoundingBoxImpl::BoundingBoxImpl(int x, int y, int w, int h) : rect_({x, y, w, h})
{
}

BoundingBoxImpl::BoundingBoxImpl(json::JSON const& json)
{
  json.Unpack("[iiii]", 0, &rect_.x, &rect_.y, &rect_.w, &rect_.h);
}

BoundingBox::BoundingBox(std::string const& filename) : impl_(new BoundingBoxImpl(json::JSON(filename)))
{
}

BoundingBox::BoundingBox(json::JSON const& json) : impl_(new BoundingBoxImpl(json))
{
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