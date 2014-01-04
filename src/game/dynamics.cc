#include "dynamics.h"
#include "timer.h"
#include "command.h"
#include "signal.h"

namespace game
{
static int const dt = 10;

class Model
{
public:
  float x_;
  float v_;
  void Step(float t)
  {
    x_ += v_ * t;
  }
};

class DynamicsImpl
{
public:
  std::pair<Model, Model> model_;
  event::Timer timer_;
  event::Signal signal_;
  DynamicsImpl(float x, float y, float u, float v);
  void Play(void);
  void Pause(void);
  void Resume(void);
  bool Step(void);
  void Add(Dynamics::Command command);
};

void DynamicsImpl::Add(Dynamics::Command command)
{
  auto bind = [command, this](void)
  {
    Dynamics::Position position(this->model_.first.x_, this->model_.second.x_);
    return command(position);
  };
  signal_.Add(bind);
}

bool DynamicsImpl::Step(void)
{
  model_.first.Step(float(dt));
  model_.second.Step(float(dt));
  signal_();
  return true;
}

DynamicsImpl::DynamicsImpl(float x, float y, float u, float v) : timer_(dt)
{
  model_.first.x_ = x;
  model_.first.v_ = u;
  model_.second.x_ = y;
  model_.second.v_ = v;
  timer_.Add(std::bind(&DynamicsImpl::Step, this));
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

Dynamics::Dynamics(void)
{
}

Dynamics::Dynamics(float x, float y, float u, float v)
{
  impl_ = std::make_shared<DynamicsImpl>(x, y, u, v);
}

void Dynamics::x(float x)
{
  impl_->model_.first.x_ = x;
}

void Dynamics::y(float y)
{
  impl_->model_.second.x_ = y;
}

void Dynamics::u(float u)
{
  impl_->model_.first.v_ = u;
}

void Dynamics::v(float v)
{
  impl_->model_.second.v_ = v;
}

float Dynamics::x(void) const
{
  return impl_->model_.first.x_;
}

float Dynamics::y(void) const
{
  return impl_->model_.second.x_;
}

float Dynamics::u(void) const
{
  return impl_->model_.first.v_;
}

float Dynamics::v(void) const
{
  return impl_->model_.second.v_;
}

void Dynamics::Play(void)
{
  impl_->Play();
}

void Dynamics::Pause(void)
{
  impl_->Pause();
}

void Dynamics::Resume(void)
{
  impl_->Resume();
}

void Dynamics::Add(Dynamics::Command const& command)
{
  impl_->Add(command);
}
}