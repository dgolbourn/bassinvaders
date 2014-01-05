#include "dynamics.h"
#include "timer.h"
#include "command.h"
#include "signal.h"
#include "mutex.h"
#include "bind.h"

namespace game
{
static int const dt = 10;

class Model
{
public:
  float x_;
  float v_;
  void Step(void)
  {
    x_ += v_ * float(dt);
  }
};

class DynamicsImpl
{
public:
  sdl::Mutex mutex_;
  std::pair<Model, Model> model_;
  event::Signal signal_;
  event::Timer timer_;
  DynamicsImpl(float x, float y, float u, float v);
  void Play(void);
  void Pause(void);
  void Resume(void);
  void Step(void);
  void Add(Dynamics::Command command);
};

void DynamicsImpl::Add(Dynamics::Command command)
{
  auto bind = [command, this](void)
  {
    sdl::Lock(this->mutex_);
    return command(Dynamics::Position(this->model_.first.x_, this->model_.second.x_));
  };
  signal_.Add(bind);
}

void DynamicsImpl::Step(void)
{
  model_.first.Step();
  model_.second.Step();
  signal_();
}

DynamicsImpl::DynamicsImpl(float x, float y, float u, float v) : timer_(dt)
{
  model_.first.x_ = x;
  model_.first.v_ = u;
  model_.second.x_ = y;
  model_.second.v_ = v;
}

void DynamicsImpl::Play(void)
{
  timer_.Play(-1);
}

void DynamicsImpl::Pause(void)
{
  timer_.Pause();
}

void DynamicsImpl::Resume(void)
{
  timer_.Resume();
}

Dynamics::Dynamics(void) noexcept
{
}

Dynamics::Dynamics(float x, float y, float u, float v)
{
  impl_ = std::make_shared<DynamicsImpl>(x, y, u, v);
  sdl::Lock(impl_->mutex_);
  impl_->timer_.Add(event::Bind(&DynamicsImpl::Step, impl_));
}

void Dynamics::x(float x)
{
  sdl::Lock(impl_->mutex_);
  impl_->model_.first.x_ = x;
}

void Dynamics::y(float y)
{
  sdl::Lock(impl_->mutex_);
  impl_->model_.second.x_ = y;
}

void Dynamics::u(float u)
{
  sdl::Lock(impl_->mutex_);
  impl_->model_.first.v_ = u;
}

void Dynamics::v(float v)
{
  sdl::Lock(impl_->mutex_);
  impl_->model_.second.v_ = v;
}

float Dynamics::x(void) const
{
  sdl::Lock(impl_->mutex_);
  return impl_->model_.first.x_;
}

float Dynamics::y(void) const
{
  sdl::Lock(impl_->mutex_);
  return impl_->model_.second.x_;
}

float Dynamics::u(void) const
{
  sdl::Lock(impl_->mutex_);
  return impl_->model_.first.v_;
}

float Dynamics::v(void) const
{
  sdl::Lock(impl_->mutex_);
  return impl_->model_.second.v_;
}

void Dynamics::Play(void)
{
  sdl::Lock(impl_->mutex_);
  impl_->Play();
}

void Dynamics::Pause(void)
{
  sdl::Lock(impl_->mutex_);
  impl_->Pause();
}

void Dynamics::Resume(void)
{
  sdl::Lock(impl_->mutex_);
  impl_->Resume();
}

void Dynamics::Add(Dynamics::Command const& command)
{
  sdl::Lock(impl_->mutex_);
  impl_->Add(command);
}
}