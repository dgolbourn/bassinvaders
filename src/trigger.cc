#include "trigger.h"
#include <utility>

namespace event
{

class TriggerImpl
{
public:
  Signal signal_;
  Callback callback_;
  int reference_count_;

  TriggerImpl(Callback callback, Signal signal);
  ~TriggerImpl(void);
};

TriggerImpl::TriggerImpl(Callback callback, Signal signal)
{
  signal_ = signal;
  signal_.Subscribe(callback);
  callback_ = callback;
  reference_count_ = 1;
}
 
TriggerImpl::~TriggerImpl(void)
{
  signal_.Unsubscribe(callback_);
  callback_ = nullptr;
}

Trigger::Trigger(Callback callback, Signal signal)
{
  impl_ = new TriggerImpl(callback, signal);
}

Trigger::Trigger(void)
{
  impl_ = nullptr;
}

Trigger::Trigger(const Trigger& original)
{
  impl_ = original.impl_;
  if(impl_)
  {
    impl_->reference_count_++;
  }
}

Trigger::Trigger(Trigger&& original)
{
  impl_ = original.impl_;
  original.impl_ = nullptr;
}

Trigger::~Trigger(void)
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

Trigger& Trigger::operator=(Trigger original)
{
  std::swap(impl_, original.impl_);
  return *this;
}

}