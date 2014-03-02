#ifndef DYNAMICS_H_
#define DYNAMICS_H_
#include <memory>
#include "weak_ptr.h"
namespace game
{
class Dynamics
{
public:
  Dynamics(void) = default;
  Dynamics(float x, float y, float u, float v, float w, float h, float c = 1.f, float m = std::numeric_limits<float>::infinity());
  void x(float x);
  void y(float y);
  void u(float u);
  void v(float v);
  void w(float w);
  void h(float h);
  void c(float c);
  void m(float m);
  void a(float a);
  void b(float b);
  void k(float k);
  void d(float d);
  float x(void) const;
  float y(void) const;
  float u(void) const;
  float v(void) const;
  float w(void) const;
  float h(void) const;
  float c(void) const;
  float m(void) const;
  float a(void) const;
  float b(void) const;
  float k(void) const;
  float d(void) const;
  void Step(float dt);
  void Collision(Dynamics const& other);
  explicit operator bool(void) const;
  typedef memory::WeakPtr<Dynamics, class DynamicsImpl> WeakPtr;
private:
  std::shared_ptr<class DynamicsImpl> impl_;
  friend WeakPtr;
};
}
#endif