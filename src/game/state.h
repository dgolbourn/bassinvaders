#ifndef STATE_H_
#define STATE_H_
#include <memory>
#include "json.h"
#include "window.h"
#include "position.h"
#include "command.h"
#include "queue.h"
namespace game
{
class State
{
public:
  State(json::JSON const& json, display::Window& window, event::Queue& queue);
  State(void) = default;
  bool operator==(State const& other) const;
  void Play(void);
  void Pause(void);
  void Resume(void);
  void Stop(void);
  void End(event::Command const& command);
  display::BoundingBox const& Collision(void) const;
  display::BoundingBox const& Render(void) const;
  void Render(display::BoundingBox const& bounding_box);
private:
  std::shared_ptr<class StateImpl> impl_;
};
}
#endif