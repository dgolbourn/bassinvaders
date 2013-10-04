#include "bounding_box.h"
#include "bounding_box_impl.h"
#include <utility>
#include "SDL_rect.h"

bool BoundingBox::operator ==(const BoundingBox& compare) const
{
  return SDL_RectEquals(&impl_->rect_, &compare.impl_->rect_);
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
  return SDL_HasIntersection(&impl_->rect_, &other.impl_->rect_);
}

bool BoundingBox::empty(void)
{
  return SDL_RectEmpty(&impl_->rect_);
}

BoundingBox::BoundingBox(void)
{
  impl_ = nullptr;
}

BoundingBox::BoundingBox(int x, int y, int w, int h)
{
  impl_ = new BoundingBoxImpl;
  impl_->rect_.x = x;
  impl_->rect_.y = y;
  impl_->rect_.w = w;
  impl_->rect_.h = h;
  impl_->reference_count_ = 1;
}

BoundingBox::BoundingBox(const BoundingBox& original)
{
  impl_ = original.impl_;
  if(impl_)
  {
    impl_->reference_count_++;
  }
}

BoundingBox::BoundingBox(BoundingBox&& original)
{
  impl_ = original.impl_;
  original.impl_ = nullptr;
}

BoundingBox::~BoundingBox(void)
{
  if(impl_)
  {
    if(impl_->reference_count_ > 0)
    {
      impl_->reference_count_--;
    }
    if(impl_->reference_count_ == 0)
    {
      delete impl_;
      impl_ = nullptr;
    }
  }
}

BoundingBox& BoundingBox::operator=(BoundingBox original)
{
  std::swap(impl_, original.impl_);
  return *this;
}
