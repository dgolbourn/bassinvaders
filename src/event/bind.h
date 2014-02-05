#ifndef BIND_H_
#define BIND_H_
#include "command.h"
namespace event
{
template<class Method, class Shared, class... Args> event::Command Bind(Method&& method, Shared const& shared, Args... args)
{
  Shared::WeakPtr weak(shared);
  return [=](void)
  {
    bool locked = false;
    if(auto shared_locked = weak.Lock())
    {
      (void)(shared_locked.*method)(args...);
      locked = true;
    }
    return locked;
  };
}

template<class Return, class Shared, class... Args> std::function<bool(Args...)> Bind(Return(Shared::*method)(Args...), Shared const& shared)
{
  Shared::WeakPtr weak(shared);
  return [=](Args... args)
  {
    bool locked = false;
    if(auto shared_locked = weak.Lock())
    {
      (void)(shared_locked.*method)(args...);
      locked = true;
    }
    return locked;
  };
}

template<class Method, class Impl, class... Args> event::Command Bind(Method&& method, std::shared_ptr<Impl> const& shared, Args... args)
{
  std::weak_ptr<Impl> weak = shared;
  return [=](void)
  {
    bool locked = false;
    if(auto shared_locked = weak.lock())
    {
      Impl* ptr = shared_locked.get();
      (void)(ptr->*method)(args...);
      locked = true;
    }
    return locked;
  };
}

template<class Return, class Impl, class... Args> std::function<std::pair<Return, bool>(Args...)> Bind(Return(Impl::*method)(Args...), std::shared_ptr<Impl> const& shared)
{
  std::weak_ptr<Impl> weak = shared;
  return [=](Args... args)
  {
    if(auto shared_locked = weak.lock())
    {
      Impl* ptr = shared_locked.get();
      return std::pair<Return, bool>((ptr->*method)(args...), true);
    }
    else
    {
      return std::pair<Return, bool>(Return(), false);
    }
  };
}

template<class Impl, class... Args> std::function<bool(Args...)> Bind(void(Impl::*method)(Args...), std::shared_ptr<Impl> const& shared)
{
  std::weak_ptr<Impl> weak = shared;
  return [=](Args... args)
  {
    if(auto shared_locked = weak.lock())
    {
      Impl* ptr = shared_locked.get();
      (ptr->*method)(args...);
      return true;
    }
    else
    {
      return false;
    }
  };
}

template<class Method, class Impl, class... Args> event::Command Bind(Method&& method, std::weak_ptr<Impl> weak, Args... args)
{
  return [=](void)
  {
    bool locked = false;
    if(auto shared_locked = weak.lock())
    {
      Impl* ptr = shared_locked.get();
      (void)(ptr->*method)(args...);
      locked = true;
    }
    return locked;
  };
}

template<class Return, class Impl, class... Args> std::function<bool(Args...)> Bind(Return(Impl::*method)(Args...), std::weak_ptr<Impl> weak)
{
  return [=](Args... args)
  {
    bool locked = false;
    if(auto shared_locked = weak.lock())
    {
      Impl* ptr = shared_locked.get();
      (void)(ptr->*method)(args...);
      locked = true;
    }
    return locked;
  };
}
}
#endif