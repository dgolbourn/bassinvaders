#include "dynamics.h"

namespace game
{
class DynamicsImpl
{
public:
  DynamicsImpl(float x, float y, float u, float v);
  void Step(float dt);
  
  float x_;
  float y_;
  float u_;
  float v_;
};

void DynamicsImpl::Step(float dt)
{
  x_ += u_ * dt;
  y_ += v_ * dt;
}

DynamicsImpl::DynamicsImpl(float x, float y, float u, float v) : x_(x), u_(u), y_(y), v_(v)
{
}

Dynamics::Dynamics(float x, float y, float u, float v)
{
  impl_ = std::make_shared<DynamicsImpl>(x, y, u, v);
}

void Dynamics::x(float x)
{
  impl_->x_ = x;
}

void Dynamics::y(float y)
{
  impl_->y_ = y;
}

void Dynamics::u(float u)
{
  impl_->u_ = u;
}

void Dynamics::v(float v)
{
  impl_->v_ = v;
}

float Dynamics::x(void) const
{
  return impl_->x_;
}

float Dynamics::y(void) const
{
  return impl_->y_;
}

float Dynamics::u(void) const
{
  return impl_->u_;
}

float Dynamics::v(void) const
{
  return impl_->v_;
}

void Dynamics::Step(float dt)
{
  impl_->Step(dt);
}
}