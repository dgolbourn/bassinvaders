#include "trigger.h"
#include <utility>

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

Trigger::Trigger(Callback& callback, Signal signal)
{
  impl_ = std::shared_ptr<TriggerImpl>(new TriggerImpl(callback, signal));
}

Trigger::Trigger(void)
{
}

Trigger::Trigger(const Trigger& original)
{
  impl_ = original.impl_;
}

Trigger::Trigger(Trigger&& original)
{
  impl_ = original.impl_;
  original.impl_.reset();
}

Trigger::~Trigger(void)
{
}

Trigger& Trigger::operator=(Trigger original)
{
  std::swap(impl_, original.impl_);
  return *this;
}

}