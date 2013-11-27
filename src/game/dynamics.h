#ifndef DYNAMICS_H_
#define DYNAMICS_H_

#include <memory>

namespace game
{
class Dynamics
{
public:
  Dynamics(void);
  Dynamics(float x, float y, float u, float v);
  float& x(void);
  float& y(void);
  float& u(void);
  float& v(void);
  void Play(void);
  void Pause(void);
  void Resume(void);

  ~Dynamics(void);
  Dynamics(Dynamics const& other);
  Dynamics(Dynamics&& other);
  Dynamics& operator=(Dynamics other);
private:
  std::shared_ptr<class DynamicsImpl> impl_;
};
}

#endif