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
  Model model_[2];
  event::Timer timer_;
  event::Signal signal_;
  DynamicsImpl(float x, float y, float u, float v);
  void Play(void);
  void Pause(void);
  void Resume(void);
  bool Step(void);
  void Add(Dynamics::Command const& command);
};

void DynamicsImpl::Add(Dynamics::Command const& command)
{
  auto bind = [command, this](void)
  {
    Dynamics::Position position(this->model_[0].x_, this->model_[1].x_);
    return command(position);
  };
  signal_.Add(bind);
}

bool DynamicsImpl::Step(void)
{
  model_[0].Step(float(dt));
  model_[1].Step(float(dt));
  signal_();
  return true;
}

DynamicsImpl::DynamicsImpl(float x, float y, float u, float v) : timer_(dt)
{
  model_[0].x_ = x;
  model_[0].v_ = u;
  model_[1].x_ = y;
  model_[1].v_ = v;
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
  impl_->model_[0].x_ = x;
}

void Dynamics::y(float y)
{
  impl_->model_[1].x_ = y;
}

void Dynamics::u(float u)
{
  impl_->model_[0].v_ = u;
}

void Dynamics::v(float v)
{
  impl_->model_[1].v_ = v;
}

float Dynamics::x(void) const
{
  return impl_->model_[0].x_;
}

float Dynamics::y(void) const
{
  return impl_->model_[1].x_;
}

float Dynamics::u(void) const
{
  return impl_->model_[0].v_;
}

float Dynamics::v(void) const
{
  return impl_->model_[1].v_;
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

Dynamics::Dynamics(Dynamics const& other) : impl_(other.impl_)
{
}

Dynamics::Dynamics(Dynamics&& other) : impl_(std::move(other.impl_))
{
}

Dynamics::~Dynamics(void)
{
}

Dynamics& Dynamics::operator=(Dynamics other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}