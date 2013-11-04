#include "signal.h"
#include <set>

namespace event
{

typedef std::set<std::weak_ptr<NotifyCommandImpl>, std::owner_less<std::weak_ptr<NotifyCommandImpl>>> NotifyCommandSet;

class SignalImpl
{
public:
  NotifyCommandSet notifies_;

  void Notify(void);
  void Subscribe(NotifyCommand const& notify);
};

void SignalImpl::Notify(void)
{
  for(auto iter = notifies_.begin(); iter != notifies_.end(); ++iter)
  {
    auto notify = iter->lock();
    if(notify)
    {
      notify->operator()();
    }
    else
    {
      notifies_.erase(iter);
    }
  }
}

void SignalImpl::Subscribe(NotifyCommand const& notify)
{
  notifies_.insert(notify);
}

void Signal::Notify(void)
{
  impl_->Notify();
}

void Signal::Subscribe(NotifyCommand const& notify)
{
  impl_->Subscribe(notify);
}

Signal::Signal(void) : impl_(new SignalImpl)
{
}

Signal::Signal(Signal const& other) : impl_(other.impl_)
{
}

Signal::Signal(Signal&& other) : impl_(std::move(other.impl_))
{
}

Signal::~Signal(void)
{
}

Signal& Signal::operator=(Signal other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

}