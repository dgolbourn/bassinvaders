#include "dynamics.h"
#include <cmath>
namespace game
{
class DynamicsImpl
{
public:
  DynamicsImpl(float x, float y, float u, float v, float w, float h, float c, float m);
  void Step(float dt);
  void Acceleration(float dt);
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
  float a_;
  float b_;
  float k_;
  float d_;
  float xk_;
  float yk_;
};

void DynamicsImpl::Acceleration(float dt)
{
  float u = 0;
  float v = 0;
  u -= std::copysign(xk_ * k_ * b_, u_);
  v -= std::copysign(yk_ * k_ * a_, v_);

  u -= d_ * u_;
  v -= d_ * v_;

  u += a_ * dt;
  v += b_ * dt;

  u_ += u;
  v_ += v;
}

void DynamicsImpl::Step(float dt)
{
  x_ += u_ * dt;
  y_ += v_ * dt;
}

static float CollisionTime(float x0, float u0, float w0, float x1, float u1, float w1)
{
  float dx = x1 - x0;
  float du = u1 - u0;
  float dw = 0.5f * (w1 + w0);
  if(du <= -0.f)
  {
    dx -= dw;
  }
  else if(du >= 0.f)
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
    if(dtx < dty && dtx > 0.f && dtx < 4.f * dt_)
    {
      Step(-dtx);
      u_ = game::Collision(u_, m_, c_, other.u_, other.m_, other.c_);
      Step(dtx);
      yk_ = other.k_;
    }
    else if(dty > 0.f && dty < 4.f * dt_)
    {
      Step(-dty);
      v_ = game::Collision(v_, m_, c_, other.v_, other.m_, other.c_);
      Step(dty);
      xk_ = other.k_;
    }
  }
}

DynamicsImpl::DynamicsImpl(float x, float y, float u, float v, float w, float h, float c, float m) : x_(x), u_(u), y_(y), v_(v), w_(w), h_(h), c_(c), m_(m), dt_(0), a_(0), b_(0), k_(0.f), d_(0.f), xk_(0.f), yk_(0.f)
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

void Dynamics::a(float a)
{
  impl_->a_ = a;
}

void Dynamics::b(float b)
{
  impl_->b_ = b;
}

void Dynamics::k(float k)
{
  impl_->k_ = k;
}

void Dynamics::d(float d)
{
  impl_->d_ = d;
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

float Dynamics::a(void) const
{
  return impl_->a_;
}

float Dynamics::b(void) const
{
  return impl_->b_;
}

float Dynamics::k(void) const
{
  return impl_->k_;
}

float Dynamics::d(void) const
{
  return impl_->d_;
}

void Dynamics::Step(float dt)
{
  impl_->Acceleration(dt);
  impl_->Step(dt);
  impl_->dt_ = dt;
  impl_->xk_ = 0.f;
  impl_->yk_ = 0.f;
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