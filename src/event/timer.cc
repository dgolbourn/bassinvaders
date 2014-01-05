#include "timer.h"
#include "SDL_timer.h"
#include "sdl_library.h"
#include "sdl_exception.h"
#include "signal.h"
#include "mutex.h"
namespace event
{
class TimerImpl
{
public:
  TimerImpl(int interval);
  void Play(int loops);
  void Pause(void);
  void Resume(void);
  void Add(Command const& command);
  void End(Command const& command);
  Uint32 Update(void);

  ~TimerImpl(void);

  sdl::Library const sdl_;
  sdl::Mutex mutex_;
  Uint32 const interval_;
  Uint32 last_update_;
  Uint32 resume_interval_;
  Signal signal_;
  Signal end_;
  int loops_;
  int max_loops_;
  bool paused_;
  SDL_TimerID timer_;
};

static SDL_TimerID const timer_null = static_cast<SDL_TimerID>(NULL);

static Uint32 TimerCallback(Uint32 interval, void* param)
{
  (void)interval;
  TimerImpl* impl = static_cast<TimerImpl*>(param);
  sdl::Lock lock(impl->mutex_);
  return impl->Update();
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

TimerImpl::TimerImpl(int interval) : sdl_(SDL_INIT_TIMER), interval_(static_cast<Uint32>(interval))
{
  timer_ = timer_null;
  paused_ = false;
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
    timer_ = timer_null;
    resume_interval_ = interval_ - SDL_GetTicks() + last_update_;
    paused_ = true;
  }
}

void TimerImpl::Play(int loops)
{
  if(timer_)
  {
    RemoveTimer(timer_);
  }
  max_loops_ = loops;
  loops_ = 0;
  timer_ = AddTimer(interval_, TimerCallback, this);
  last_update_ = SDL_GetTicks();
  paused_ = false;
}

void TimerImpl::Resume(void)
{
  if(paused_)
  {
    last_update_ = SDL_GetTicks() - interval_ + resume_interval_;
    timer_ = AddTimer(resume_interval_, TimerCallback, this);
    paused_ = false;
  }
}

void TimerImpl::Add(event::Command const& command)
{
  return signal_.Add(command);
}

void TimerImpl::End(event::Command const& command)
{
  return end_.Add(command);
}

Uint32 TimerImpl::Update(void)
{
  signal_();
  last_update_ = SDL_GetTicks();
  
  int interval = interval_;
  if(max_loops_ >= 0)
  {
    if(loops_ == max_loops_)
    {
      end_();
      interval = 0;
      timer_ = timer_null;
    }
    ++loops_;
  }
  return interval;
}

Timer::Timer(void)
{
}

Timer::Timer(int interval)
{
  impl_ = std::make_shared<TimerImpl>(interval);
}

void Timer::Pause(void)
{
  sdl::Lock lock(impl_->mutex_);
  impl_->Pause();
}

void Timer::Resume(void)
{
  sdl::Lock lock(impl_->mutex_);
  impl_->Resume();
}

void Timer::Add(event::Command const& command)
{
  sdl::Lock lock(impl_->mutex_);
  return impl_->Add(command);
}

void Timer::End(event::Command const& command)
{
  sdl::Lock lock(impl_->mutex_);
  impl_->End(command);
}

void Timer::Play(int loops)
{
  sdl::Lock lock(impl_->mutex_);
  impl_->Play(loops);
}
}