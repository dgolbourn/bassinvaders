#include "timer.h"
#include "SDL_timer.h"
#include "sdl_library.h"
#include "sdl_exception.h"
#include "signal.h"
#include "thread.h"
namespace event
{
class TimerImpl
{
public:
  TimerImpl(int interval, event::Queue& queue);
  void Play(int loops);
  void Pause(void);
  void Resume(void);
  void Add(Command const& command);
  void End(Command const& command);
  Uint32 Interval(void);
  Uint32 Update(void);

  ~TimerImpl(void);

  sdl::Library const sdl_;
  std::mutex mutex_;
  Uint32 const interval_;
  Uint32 last_update_;
  Uint32 resume_interval_;
  Signal signal_;
  Signal end_;
  int loops_;
  int max_loops_;
  bool paused_;
  SDL_TimerID timer_;
  event::Queue queue_;
};

static SDL_TimerID const timer_null = static_cast<SDL_TimerID>(NULL);

static Uint32 TimerCallback(Uint32, void* param)
{
  TimerImpl* impl = static_cast<TimerImpl*>(param);
  Uint32 interval;
  std::unique_lock<std::mutex> lock(impl->mutex_, std::defer_lock);
  if(-1 == std::try_lock(lock))
  {
    interval = impl->Update();
  }
  else
  {
    interval = 1;
  }
  return interval;
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

TimerImpl::TimerImpl(int interval, event::Queue& queue) : sdl_(SDL_INIT_TIMER), interval_(static_cast<Uint32>(interval)), queue_(queue)
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

Uint32 TimerImpl::Interval(void)
{
  Uint32 interval = interval_ + last_update_ - SDL_GetTicks();
  if(interval == 0u || interval > interval_)
  {
    interval = 1;
  }
  return interval;
}

void TimerImpl::Pause(void)
{
  if(timer_)
  {
    RemoveTimer(timer_);
    timer_ = timer_null;
    resume_interval_ = Interval();
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
  Uint32 current_time = SDL_GetTicks();
  Uint32 update_interval = current_time - last_update_;
  
  bool end_flag = false;
  while(update_interval >= interval_)
  {
    update_interval -= interval_;
    if(max_loops_ < 0)
    {
      signal_(queue_);
    }
    else
    {
      if(loops_ <= max_loops_)
      {
        signal_(queue_);
      }
      if(loops_ >= max_loops_)
      {
        end_flag = true;
        break;
      }
      ++loops_;
    }
  }

  last_update_ = current_time - update_interval;

  Uint32 interval;
  if(end_flag)
  {
    end_(queue_);
    timer_ = timer_null;
    interval = 0;
  }
  else
  {
    interval = Interval();
  }
  return interval;
}

Timer::Timer(int interval, event::Queue& queue)
{
  impl_ = std::make_shared<TimerImpl>(interval, queue);
}

void Timer::Pause(void)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Pause();
}

void Timer::Resume(void)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Resume();
}

void Timer::Add(event::Command const& command)
{
  thread::Lock lock(impl_->mutex_);
  return impl_->Add(command);
}

void Timer::End(event::Command const& command)
{
  thread::Lock lock(impl_->mutex_);
  impl_->End(command);
}

void Timer::Play(int loops)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Play(loops);
}
}