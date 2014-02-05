#ifndef DYNAMICS_H_
#define DYNAMICS_H_
#include <memory>
namespace game
{
class Dynamics
{
public:
  Dynamics(void) = default;
  Dynamics(float x, float y, float u, float v);
  void x(float x);
  void y(float y);
  void u(float u);
  void v(float v);
  float x(void) const;
  float y(void) const;
  float u(void) const;
  float v(void) const;
  void Step(float dt);
private:
  std::shared_ptr<class DynamicsImpl> impl_;
};
}
#endif