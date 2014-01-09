#include "bounding_box.h"
#include "SDL_rect.h"
#include "rect.h"
#include "thread.h"

namespace display
{
class BoundingBoxImpl
{
public:
  BoundingBoxImpl(int x, int y, int w, int h);
  BoundingBoxImpl(json::JSON const& json);
  std::mutex mutex_; 
  SDL_Rect rect_;
};

bool BoundingBox::operator&&(BoundingBox const& other) const
{
  std::lock(impl_->mutex_, other.impl_->mutex_);
  bool intersection = sdl::Intersection(&impl_->rect_, &other.impl_->rect_);
  other.impl_->mutex_.unlock();
  impl_->mutex_.unlock();
  return intersection;
}

bool BoundingBox::operator<(BoundingBox const& other) const
{
  std::lock(impl_->mutex_, other.impl_->mutex_);
  bool less = impl_.owner_before(other.impl_);
  other.impl_->mutex_.unlock();
  impl_->mutex_.unlock();
  return less;
}

BoundingBox::operator bool(void) const
{
  return bool(impl_);
}

BoundingBox::operator SDL_Rect(void) const
{
  thread::Lock(impl_->mutex_);
  return impl_->rect_;
}

void BoundingBox::x(int x)
{
  thread::Lock(impl_->mutex_);
  impl_->rect_.x = x;
}

void BoundingBox::y(int y)
{
  thread::Lock(impl_->mutex_);
  impl_->rect_.y = y;
}

void BoundingBox::w(int w)
{
  thread::Lock(impl_->mutex_);
  impl_->rect_.w = w;
}

void BoundingBox::h(int h)
{
  thread::Lock(impl_->mutex_);
  impl_->rect_.h = h;
}

int BoundingBox::x(void) const
{
  thread::Lock(impl_->mutex_);
  return impl_->rect_.x;
}

int BoundingBox::y(void) const
{
  thread::Lock(impl_->mutex_);
  return impl_->rect_.y;
}

int BoundingBox::w(void) const
{
  thread::Lock(impl_->mutex_);
  return impl_->rect_.w;
}

int BoundingBox::h(void) const
{
  thread::Lock(impl_->mutex_);
  return impl_->rect_.h;
}

BoundingBox BoundingBox::Copy(void) const
{
  thread::Lock(impl_->mutex_);
  return BoundingBox(impl_->rect_.x, impl_->rect_.y, impl_->rect_.w, impl_->rect_.h);
}

void BoundingBox::Copy(BoundingBox const& other) const
{
  std::lock(impl_->mutex_, other.impl_->mutex_);
  impl_->rect_ = other.impl_->rect_;
  other.impl_->mutex_.unlock();
  impl_->mutex_.unlock();
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

BoundingBox::BoundingBox(json::JSON const& json)
{
  impl_ = std::make_shared<BoundingBoxImpl>(json);
}
}