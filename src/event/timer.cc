#include "timer.h"
#include "SDL_timer.h"
#include "sdl_manager.h"
#include "sdl_exception.h"
#include "signal.h"

namespace event
{
class TimerImpl
{
public:
  TimerImpl(int interval);
  void Pause(void);
  void Resume(void);
  void Add(Command const& command);
  Uint32 Update(void);

  ~TimerImpl(void);

  Uint32 interval_;
  Uint32 last_update_;
  Uint32 resume_interval_;
  SDL_TimerID timer_;
  Signal signal_;
  sdl::Library const sdl_;
};

static Uint32 TimerCallback(Uint32 interval, void* param)
{
  return static_cast<TimerImpl*>(param)->Update();
}

static SDL_TimerID AddTimer(Uint32 interval, SDL_TimerCallback callback, void* param)
{
  SDL_TimerID timer = SDL_AddTimer(interval, callback, param);
  if(!timer)
  {
    throw sdl::Exception();
  }
  return timer;
}

static void RemoveTimer(SDL_TimerID id)
{
  if(!SDL_RemoveTimer(id))
  {
    throw sdl::Exception();
  }
}

TimerImpl::TimerImpl(int interval) : sdl_(SDL_INIT_TIMER)
{
  timer_ = AddTimer(interval, TimerCallback, this);
  interval_ = static_cast<Uint32>(interval);
  last_update_ = SDL_GetTicks();
}

TimerImpl::~TimerImpl(void)
{
  if(timer_)
  {
    RemoveTimer(timer_);
  }
}

void TimerImpl::Pause(void)
{
  if(timer_)
  {
    RemoveTimer(timer_);
    timer_ = static_cast<SDL_TimerID>(NULL);
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
    RemoveTimer(timer_);
    interval = interval_;
    last_update_ = SDL_GetTicks();
  }
  timer_ = AddTimer(interval, TimerCallback, this);
}

void TimerImpl::Add(event::Command const& command)
{
  return signal_.Add(command);
}

Uint32 TimerImpl::Update(void)
{
  signal_.Notify();
  last_update_ = SDL_GetTicks();
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

void Timer::Add(event::Command const& command)
{
  return impl_->Add(command);
}
}