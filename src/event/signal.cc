#include "signal.h"
#include <set>

namespace event
{
typedef std::owner_less<CommandPtr> CommandLess;
typedef std::set<CommandPtr, CommandLess> CommandSet;

class SignalImpl
{
public:
  void Notify(void);
  void Add(Command const& comand);
  CommandSet commands_;
};

void SignalImpl::Notify(void)
{
  for(auto iter = commands_.begin(); iter != commands_.end();)
  {
    if(auto command = iter->lock())
    {
      command->operator()();
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
  commands_.insert(comand);
}

void Signal::Notify(void)
{
  impl_->Notify();
}

void Signal::Add(Command const& comand)
{
  impl_->Add(comand);
}

Signal::Signal(void) : impl_(new SignalImpl())
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