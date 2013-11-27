#include "dynamics.h"
#include "timer.h"
#include "command.h"

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
  event::Command update_;
  DynamicsImpl(float x, float y, float u, float v);
  void Play(void);
  void Pause(void);
  void Resume(void);
};

class DynamicsCommand : public event::CommandImpl
{
public:
  DynamicsImpl& impl_;
  DynamicsCommand(DynamicsImpl& impl);
  void operator()(void) final;
};

void DynamicsCommand::operator()(void)
{
  impl_.model_[0].Step(float(dt));
  impl_.model_[1].Step(float(dt));
}

DynamicsCommand::DynamicsCommand(DynamicsImpl& impl) : impl_(impl)
{
}

DynamicsImpl::DynamicsImpl(float x, float y, float u, float v) : timer_(dt)
{
  model_[0].x_ = x;
  model_[0].v_ = u;
  model_[1].x_ = y;
  model_[1].v_ = v;
  update_ = event::Command(new DynamicsCommand(*this));
  timer_.Add(update_);
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

Dynamics::Dynamics(float x, float y, float u, float v) : impl_(new DynamicsImpl(x, y, u, v))
{
}
  
float& Dynamics::x(void)
{
  return impl_->model_[0].x_;
}

float& Dynamics::y(void)
{
  return impl_->model_[1].x_;
}

float& Dynamics::u(void)
{
  return impl_->model_[0].v_;
}

float& Dynamics::v(void)
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