#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <memory>
#include <string>
#include "window.h"

namespace game
{
class Animation
{
public:
  Animation(std::string& filename, display::Window& window);
  Animation(void);
  ~Animation(void);
  Animation::Animation(Animation const& other);
  Animation(Animation&& other);
  Animation& operator=(Animation other);
  
  void Render(display::BoundingBox& destination);
  void Pause(void);
  void Resume(void);
  void Restart(void);
private:
  std::shared_ptr<class AnimationImpl> impl_;
};
}
#endif