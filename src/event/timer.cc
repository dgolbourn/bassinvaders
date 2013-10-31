#include "timer.h"
#include "SDL_timer.h"
#include "sdl_manager.h"

namespace event
{

class TimerImpl
{
public:
  Uint32 paused_start_;
  Uint32 paused_time_;

  SDL_TimerID timer_;

  Uint32 interval_;
  bool repeats_;

  Signal signal_;

  TimerImpl(int interval, bool repeats);
  ~TimerImpl(void);
  void Pause(void);
  void Resume(void);
  void Restart(void);
  Signal Signal(void);
  Uint32 Update(void);
};

static Uint32 TimerCallback(Uint32 interval, void* param)
{
  return ((TimerImpl*)param)->Update();
}

TimerImpl::TimerImpl(int interval, bool repeats)
{
  sdl::Init(SDL_INIT_TIMER);
  repeats_ = repeats;
  interval_ = (Uint32)interval;
  timer_ = SDL_AddTimer(interval_, TimerCallback, this);
  paused_start_ = 0;
  paused_time_ = 0;
}

TimerImpl::~TimerImpl(void)
{
  SDL_RemoveTimer(timer_);
  sdl::Quit(SDL_INIT_TIMER);
}

void TimerImpl::Pause(void)
{
  if(!paused_start_)
  {
    paused_start_ = SDL_GetTicks();
  }
}

void TimerImpl::Resume(void)
{
  if(paused_start_)
  {
    paused_time_ += SDL_GetTicks() - paused_start_;
    paused_start_ = 0;
  }
}

void TimerImpl::Restart(void)
{
  SDL_RemoveTimer(timer_);
  timer_ = SDL_AddTimer(interval_, TimerCallback, this);
  paused_start_ = 0;
  paused_time_ = 0;
}

Signal TimerImpl::Signal(void)
{
  return signal_;
}

Uint32 TimerImpl::Update(void)
{
  Uint32 interval;
  if(paused_start_)
  {
    interval = 1;
  }
  else
  {
    if(paused_time_)
    {
      interval = paused_time_;
    }
    else
    {
      signal_.Notify();
      if(repeats_)
      {
        interval = interval_;
      }
      else
      {
        interval = 0;
      }
    }
    paused_time_ = 0;
  }
  return interval;
}

Timer::Timer(void)
{
}

Timer::Timer(int interval, bool repeats) : impl_(new TimerImpl(interval, repeats))
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

void Timer::Restart(void)
{
  impl_->Restart();
}

Signal Timer::Signal(void)
{
  return impl_->Signal();
}

}