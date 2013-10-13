#include "signal.h"
#include <set>

namespace event
{

class SignalImpl
{
public:
  std::set<Callback> callbacks_;

  SignalImpl(void);
  ~SignalImpl(void);
  void Emit(Signal signal);
  void Subscribe(Callback callback);
  void Unsubscribe(Callback callback);
};

SignalImpl::SignalImpl(void)
{
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
  impl_->Emit(*this);
}

void Signal::Subscribe(Callback callback)
{
  impl_->Subscribe(callback);
}

void Signal::Unsubscribe(Callback callback)
{
  impl_->Unsubscribe(callback);
}

Signal::Signal(void)
{
  impl_ = std::shared_ptr<SignalImpl>(new SignalImpl());
}

Signal::Signal(const Signal& original)
{
  impl_ = original.impl_;
}

Signal::Signal(Signal&& original)
{
  impl_ = original.impl_;
  original.impl_.reset();
}

Signal::~Signal(void)
{
}

Signal& Signal::operator=(Signal original)
{
  std::swap(impl_, original.impl_);
  return *this;
}

}