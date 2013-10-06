#include "listener.h"
#include <utility>

class ListenerImpl
{
public:
  Signal signal_;
  Callback callback_;
  int reference_count_;

  ListenerImpl(Callback callback, Signal signal);
  ~ListenerImpl(void);
};

ListenerImpl::ListenerImpl(Callback callback, Signal signal)
{
  signal_ = signal;
  signal_.Subscribe(callback);
  callback_ = callback;
  reference_count_ = 1;
}
 
ListenerImpl::~ListenerImpl(void)
{
  signal_.Unsubscribe(callback_);
  callback_ = nullptr;
}

Listener::Listener(Callback callback, Signal signal)
{
  impl_ = new ListenerImpl(callback, signal);
}

Listener::Listener(void)
{
  impl_ = nullptr;
}

Listener::Listener(const Listener& original)
{
  impl_ = original.impl_;
  if(impl_)
  {
    impl_->reference_count_++;
  }
}

Listener::Listener(Listener&& original)
{
  impl_ = original.impl_;
  original.impl_ = nullptr;
}

Listener::~Listener(void)
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

Listener& Listener::operator=(Listener original)
{
  std::swap(impl_, original.impl_);
  return *this;
}