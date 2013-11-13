#include "bounding_box.h"
#include "bounding_box_impl.h"
#include "SDL_rect.h"

namespace display
{
void BoundingBox::Reset(void)
{
  impl_.reset();
}

bool BoundingBox::operator==(BoundingBox const& other) const
{
  return impl_ == other.impl_;
}

bool BoundingBox::operator&&(BoundingBox const& other) const
{
  return SDL_HasIntersection(&impl_->rect_, &other.impl_->rect_) != SDL_FALSE;
}

bool BoundingBox::operator<(BoundingBox const& other) const
{
  return impl_ < other.impl_;
}

BoundingBox::operator bool(void) const
{
  return impl_ && (SDL_RectEmpty(&impl_->rect_) == SDL_FALSE);
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

BoundingBox::BoundingBox(int x, int y, int w, int h)
{
  impl_ = std::shared_ptr<BoundingBoxImpl>(new BoundingBoxImpl);
  impl_->rect_.x = x;
  impl_->rect_.y = y;
  impl_->rect_.w = w;
  impl_->rect_.h = h;
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