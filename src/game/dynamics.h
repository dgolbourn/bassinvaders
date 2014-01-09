#ifndef DYNAMICS_H_
#define DYNAMICS_H_

#include <memory>
#include <functional>

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
  void Play(void);
  void Pause(void);
  void Resume(void);
  typedef std::pair<float, float> Position;
  typedef std::function<bool(Position const&)> Command;
  void Add(Command const& command);
private:
  std::shared_ptr<class DynamicsImpl> impl_;
};
}

#endif