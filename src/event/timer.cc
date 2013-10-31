#include "timer.h"
#include "SDL_timer.h"
#include "sdl_manager.h"
#include "sdl_exception.h"

namespace event
{

class TimerImpl
{
public:
  Uint32 interval_;

  Uint32 last_update_;
  Uint32 resume_interval_;

  SDL_TimerID timer_;

  Signal signal_;

  TimerImpl(int interval);
  ~TimerImpl(void);
  void Pause(void);
  void Resume(void);
  Signal Signal(void);
  Uint32 Update(void);
};

static Uint32 TimerCallback(Uint32 interval, void* param)
{
  return static_cast<TimerImpl*>(param)->Update();
}

TimerImpl::TimerImpl(int interval)
{
  sdl::Init(SDL_INIT_TIMER);
  timer_ = SDL_AddTimer(interval, TimerCallback, this);
  if(!timer_)
  {
    throw sdl::Exception();
  }
  interval_ = (Uint32)interval;
  last_update_ = SDL_GetTicks();
}

TimerImpl::~TimerImpl(void)
{
  if(timer_)
  {
    if(!SDL_RemoveTimer(timer_))
    {
      throw sdl::Exception();
    }
  }
  sdl::Quit(SDL_INIT_TIMER);
}

void TimerImpl::Pause(void)
{
  if(timer_)
  {
    if(!SDL_RemoveTimer(timer_))
    {
      throw sdl::Exception();
    }
    timer_ = NULL;
    resume_interval_ = interval_ - SDL_GetTicks() + last_update_;
  }
}

void TimerImpl::Resume(void)
{
  Uint32 interval;
  if(!timer_)
  {
    interval = resume_interval_;
    last_update_ = SDL_GetTicks() - interval_ + resume_interval_;
  }
  else
  {
    if(!SDL_RemoveTimer(timer_))
    {
      throw sdl::Exception();
    }
    interval = interval_;
    last_update_ = SDL_GetTicks();
  }
  timer_ = SDL_AddTimer(interval, TimerCallback, this);
  if(!timer_)
  {
    throw sdl::Exception();
  }
}

Signal TimerImpl::Signal(void)
{
  return signal_;
}

Uint32 TimerImpl::Update(void)
{
  signal_.Notify();
  return interval_;
}

Timer::Timer(void)
{
}

Timer::Timer(int interval) : impl_(new TimerImpl(interval))
{
}

Timer::~Timer(void)
{
}

Timer::Timer(Timer const& other) : impl_(other.impl_)
{
}

Timer::Timer(Timer&& other) : impl_(std::move(other.impl_))
{
}

Timer& Timer::operator=(Timer other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

void Timer::Pause(void)
{
  impl_->Pause();
}

void Timer::Resume(void)
{
  impl_->Resume();
}

Signal Timer::Signal(void)
{
  return impl_->Signal();
}

}