#include "trigger.h"

namespace event
{

class TriggerImpl
{
public:
  Signal signal_;
  Callback* callback_;

  TriggerImpl(Callback& callback, Signal signal);
  ~TriggerImpl(void);
};

TriggerImpl::TriggerImpl(Callback& callback, Signal signal)
{
  signal_ = signal;
  signal_.Subscribe(callback);
  callback_ = &callback;
}
 
TriggerImpl::~TriggerImpl(void)
{
  signal_.Unsubscribe(*callback_);
}

Trigger::Trigger(Callback& callback, Signal signal) : impl_(new TriggerImpl(callback, signal))
{
}

Trigger::Trigger(void)
{
}

Trigger::Trigger(Trigger const& other) : impl_(other.impl_)
{
}

Trigger::Trigger(Trigger&& other) : impl_(std::move(other.impl_))
{
}

Trigger::~Trigger(void)
{
}

Trigger& Trigger::operator=(Trigger other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

}