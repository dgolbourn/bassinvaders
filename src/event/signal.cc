#include "signal.h"
#include <set>

namespace event
{

typedef std::set<std::weak_ptr<Notification>, std::owner_less<std::weak_ptr<Notification>>> ObserverSet;

class SignalImpl
{
public:
  ObserverSet observers_;

  void Notify(void);
  void Subscribe(Observer const& observer);
};

void SignalImpl::Notify(void)
{
  for(auto iter = observers_.begin(); iter != observers_.end(); ++iter)
  {
    auto observer = iter->lock();
    if(observer)
    {
      observer->operator()();
    }
    else
    {
      observers_.erase(iter);
    }
  }
}

void SignalImpl::Subscribe(Observer const& observer)
{
  observers_.insert(observer);
}

void Signal::Notify(void)
{
  impl_->Notify();
}

void Signal::Subscribe(Observer const& observer)
{
  impl_->Subscribe(observer);
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