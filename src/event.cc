#include "event.h"
#include <set>

class EventImpl
{
public:
  std::set<Callback> callbacks_;
  int reference_count_;

  EventImpl(void);
  ~EventImpl(void);
  void Emit(Event event);
  void Subscribe(Callback callback);
  void Unsubscribe(Callback callback);
};

EventImpl::EventImpl(void)
{
  reference_count_ = 1;
}

EventImpl::~EventImpl(void)
{
}

void EventImpl::Emit(Event event)
{
  for(auto iter = callbacks_.begin(); iter != callbacks_.end(); ++iter)
  {
    Callback callback = *iter;
    if(callback)
    {
      callback(event);
    }
  }
}

void EventImpl::Subscribe(Callback callback)
{
  callbacks_.insert(callback);
}

void EventImpl::Unsubscribe(Callback callback)
{
  auto iter = callbacks_.find(callback);
  if(iter != callbacks_.end())
  {
    callbacks_.erase(iter);
  }
}

void Event::Emit(void)
{
  if(impl_)
  {
    impl_->Emit(*this);
  }
}

void Event::Subscribe(Callback callback)
{
  if(impl_)
  {
    impl_->Subscribe(callback);
  }
}

void Event::Unsubscribe(Callback callback)
{
  if(impl_)
  {
    impl_->Unsubscribe(callback);
  }
}

Event::Event(void)
{
  impl_ = new EventImpl();
}

Event::Event(const Event& original)
{
  impl_ = original.impl_;
  if(impl_)
  {
    impl_->reference_count_++;
  }
}

Event::Event(Event&& original)
{
  impl_ = original.impl_;
  original.impl_ = nullptr;
}

Event::~Event(void)
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

Event& Event::operator=(Event original)
{
  std::swap(impl_, original.impl_);
  return *this;
}