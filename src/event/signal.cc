#include "signal.h"
#include <list>
#include "bind.h"
#include "thread.h"
namespace event
{
typedef std::list<Command> CommandList;

class SignalImpl
{
public:
  void Notify(void);
  void Add(Command const& comand);
  std::mutex mutex_;
  CommandList commands_;
};

void SignalImpl::Notify(void)
{
  for(auto iter = commands_.begin(); iter != commands_.end();)
  {
    if((*iter)())
    {
      ++iter;
    }
    else
    {
      iter = commands_.erase(iter);
    }
  }
}

void SignalImpl::Add(Command const& comand)
{
  commands_.push_back(comand);
}

void Signal::operator()(Queue& queue)
{
  thread::Lock lock(impl_->mutex_);
  queue.Add(event::Bind(&SignalImpl::Notify, impl_));
}

void Signal::operator()(void)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Notify();
}

void Signal::Add(Command const& comand)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Add(comand);
}

Signal::Signal(void)
{
  impl_ = std::make_shared<SignalImpl>();
}
}