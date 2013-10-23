#include "bounding_box.h"
#include "bounding_box_impl.h"
#include "SDL_rect.h"

namespace display
{

bool BoundingBox::operator ==(BoundingBox const& compare) const
{
  return SDL_RectEquals(&impl_->rect_, &compare.impl_->rect_) != 0;
}

BoundingBox BoundingBox::operator |(BoundingBox const& other)
{
  BoundingBox intersection;
  (void)SDL_IntersectRect(&impl_->rect_, 
                          &other.impl_->rect_,
                          &intersection.impl_->rect_);
  return intersection;
}

BoundingBox BoundingBox::operator &(BoundingBox const& other)
{
  BoundingBox complement;
  SDL_UnionRect(&impl_->rect_, 
                &other.impl_->rect_,
                &complement.impl_->rect_);
  return complement;
}

bool BoundingBox::operator &&(BoundingBox const& other)
{
  return SDL_HasIntersection(&impl_->rect_, &other.impl_->rect_) != 0;
}

bool BoundingBox::Empty(void)
{
  return SDL_RectEmpty(&impl_->rect_) != 0;
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