#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <memory>
#include <string>
#include "window.h"
#include "json.h"

namespace game
{
class Animation
{
public:
  Animation(std::string const& filename, display::Window& window);
  Animation(json::JSON const& json, display::Window& window);
  Animation(void);
  void Render(display::BoundingBox const& destination);
  void Pause(void);
  void Resume(void);
  void Restart(void);

  ~Animation(void);
  Animation::Animation(Animation const& other);
  Animation(Animation&& other);
  Animation& operator=(Animation other);
private:
  std::shared_ptr<class AnimationImpl> impl_;
};
}
#endif