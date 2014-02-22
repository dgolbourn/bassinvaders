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

  bool operator<(WeakPtr const& other) const
  {
    return impl_.owner_before(other.impl_);
  }
};
}
#endif