#include "bounding_box.h"
#include "bounding_box_impl.h"
#include "SDL_rect.h"

namespace display
{

bool BoundingBox::operator ==(const BoundingBox& compare) const
{
  return SDL_RectEquals(&impl_->rect_, &compare.impl_->rect_) != 0;
}

BoundingBox BoundingBox::operator |(const BoundingBox& other)
{
  BoundingBox intersection;
  (void)SDL_IntersectRect(&impl_->rect_, 
                          &other.impl_->rect_,
                          &intersection.impl_->rect_);
  return intersection;
}

BoundingBox BoundingBox::operator &(const BoundingBox& other)
{
  BoundingBox complement;
  SDL_UnionRect(&impl_->rect_, 
                &other.impl_->rect_,
                &complement.impl_->rect_);
  return complement;
}

bool BoundingBox::operator &&(const BoundingBox& other)
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

BoundingBox::BoundingBox(const BoundingBox& original)
{
  impl_ = original.impl_;
}

BoundingBox::BoundingBox(BoundingBox&& original)
{
  impl_ = original.impl_;
  original.impl_.reset();
}

BoundingBox::~BoundingBox(void)
{
}

BoundingBox& BoundingBox::operator=(BoundingBox original)
{
  std::swap(impl_, original.impl_);
  return *this;
}

}