#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <memory>
#include <string>
#include "window.h"
#include "json.h"
#include "command.h"
#include "queue.h"

namespace game
{
class Animation
{
public:
  Animation(json::JSON const& json, display::Window& window, event::Queue& queue);
  Animation(void) = default;
  void Render(display::BoundingBox const& destination, float parallax = 0.f, bool tile = false, double angle = 0.) const;
  void Pause(void);
  void Resume(void);
  void Play(int loops = 0, bool end_on_first = false);
  void End(event::Command const& command);
private:
  std::shared_ptr<class AnimationImpl> impl_;
};
}
#endif