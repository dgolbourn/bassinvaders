#ifndef BIND_H_
#define BIND_H_
#include "command.h"
namespace event
{
template<class SharedMethod, class Shared, class... Args> event::Command Bind(SharedMethod&& method, Shared const& shared, Args... args)
{
  Shared::WeakPtr weak(shared);
  return [=](void)
  {
    bool locked = false;
    if(auto shared_locked = weak.Lock())
    {
      (shared_locked.*method)(args...);
      locked = true;
    }
    return locked;
  };
}
}
#endif