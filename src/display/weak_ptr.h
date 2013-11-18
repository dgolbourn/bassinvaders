#ifndef WEAK_PTR_H_
#define WEAK_PTR_H_

#include <memory>
namespace memory
{
template<class T, class TImpl> class WeakPtr
{
  std::weak_ptr<TImpl> impl_;
public:
  WeakPtr(void)
  {
  }

  WeakPtr(T const& object) : impl_(object.impl_)
  {
  }

  T Lock(void) const
  {
    T object;
    object.impl_ = impl_.lock();
    return object;
  }

  bool owner_before(WeakPtr const& other) const
  {
    return impl_.owner_before(other.impl_);
  }

  bool operator<(WeakPtr const& other) const
  {
    return impl_.owner_before(other.impl_);
  }

  WeakPtr(WeakPtr const& other) : impl_(other.impl_)
  {
  }

  WeakPtr(WeakPtr&& other) : impl_(std::move(other.impl_))
  {
  }

  ~WeakPtr(void)
  {
  }

  WeakPtr& operator=(WeakPtr other)
  {
    std::swap(impl_, other.impl_);
    return *this;
  }
};
}
#endif