#include "dynamics.h"
#include <cmath>
namespace game
{
class DynamicsImpl
{
public:
  DynamicsImpl(float x, float y, float u, float v, float w, float h, float c, float m);
  void Step(float dt);
  void Collision(DynamicsImpl const& other);
  float x_;
  float y_;
  float u_;
  float v_;
  float w_;
  float h_;
  float c_;
  float m_;
  float dt_;
};

void DynamicsImpl::Step(float dt)
{
  x_ += u_ * dt;
  y_ += v_ * dt;
  dt_ = dt;
}

static float CollisionTime(float x0, float u0, float w0, float x1, float u1, float w1)
{
  float dx = x1 - x0;
  float du = u0 - u1;
  float dw = 0.5f * (w1 + w0);
  if(du > 0.f)
  {
    dx -= dw;
  }
  else
  {
    dx += dw;
  }
  return dx / du;
}

static float Collision(float u0, float m0, float c0, float u1, float m1, float c1)
{
  float v;
  if(std::isfinite(m1))
  {
    v = (c0 * c1 * m1 * (u1 - u0) + m0 * u0 + m1 * u1) / (m0 + m1);
  }
  else
  {
    v = -c0 * c1 * u0;
  }
  return v;
}

void DynamicsImpl::Collision(DynamicsImpl const& other)
{
  if(std::isfinite(m_))
  {
    float dtx = CollisionTime(x_, u_, w_, other.x_, other.u_, other.w_);
    float dty = CollisionTime(y_, v_, h_, other.y_, other.v_, other.h_);
    if(dtx < dty && dtx > 0 && dtx < dt_)
    {
      Step(-dtx);
      u_ = game::Collision(u_, m_, c_, other.u_, other.m_, other.c_);
      Step(dtx);
    }
    else if(dty > 0 && dty < dt_)
    {
      Step(-dty);
      v_ = game::Collision(v_, m_, c_, other.v_, other.m_, other.c_);
      Step(dty);
    }
  }
}

DynamicsImpl::DynamicsImpl(float x, float y, float u, float v, float w, float h, float c, float m) : x_(x), u_(u), y_(y), v_(v), w_(w), h_(h), c_(c), m_(m), dt_(0)
{
}

Dynamics::Dynamics(float x, float y, float u, float v, float w, float h, float c, float m)
{
  impl_ = std::make_shared<DynamicsImpl>(x, y, u, v, w, h, c, m);
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

void Dynamics::w(float w)
{
  impl_->w_ = w;
}

void Dynamics::h(float h)
{
  impl_->h_ = h;
}

void Dynamics::c(float c)
{
  impl_->c_ = c;
}

void Dynamics::m(float m)
{
  impl_->m_ = m;
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

float Dynamics::w(void) const
{
  return impl_->w_;
}

float Dynamics::h(void) const
{
  return impl_->h_;
}

float Dynamics::c(void) const
{
  return impl_->c_;
}

float Dynamics::m(void) const
{
  return impl_->m_;
}

void Dynamics::Step(float dt)
{
  impl_->Step(dt);
}

void Dynamics::Collision(Dynamics const& other)
{
  impl_->Collision(*other.impl_);
}

Dynamics::operator bool(void) const
{
  return bool(impl_);
}
}