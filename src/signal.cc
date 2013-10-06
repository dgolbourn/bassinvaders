#include "signal.h"
#include <set>

class SignalImpl
{
public:
  std::set<Callback> callbacks_;
  int reference_count_;

  SignalImpl(void);
  ~SignalImpl(void);
  void Emit(Signal signal);
  void Subscribe(Callback callback);
  void Unsubscribe(Callback callback);
};

SignalImpl::SignalImpl(void)
{
  reference_count_ = 1;
}

SignalImpl::~SignalImpl(void)
{
}

void SignalImpl::Emit(Signal signal)
{
  for(auto iter = callbacks_.begin(); iter != callbacks_.end(); ++iter)
  {
    Callback callback = *iter;
    if(callback)
    {
      callback(signal);
    }
  }
}

void SignalImpl::Subscribe(Callback callback)
{
  callbacks_.insert(callback);
}

void SignalImpl::Unsubscribe(Callback callback)
{
  auto iter = callbacks_.find(callback);
  if(iter != callbacks_.end())
  {
    callbacks_.erase(iter);
  }
}

void Signal::Emit(void)
{
  if(impl_)
  {
    impl_->Emit(*this);
  }
}

void Signal::Subscribe(Callback callback)
{
  if(impl_)
  {
    impl_->Subscribe(callback);
  }
}

void Signal::Unsubscribe(Callback callback)
{
  if(impl_)
  {
    impl_->Unsubscribe(callback);
  }
}

Signal::Signal(void)
{
  impl_ = new SignalImpl();
}

Signal::Signal(const Signal& original)
{
  impl_ = original.impl_;
  if(impl_)
  {
    impl_->reference_count_++;
  }
}

Signal::Signal(Signal&& original)
{
  impl_ = original.impl_;
  original.impl_ = nullptr;
}

Signal::~Signal(void)
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

Signal& Signal::operator=(Signal original)
{
  std::swap(impl_, original.impl_);
  return *this;
}