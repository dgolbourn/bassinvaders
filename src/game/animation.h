#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <memory>

namespace game
{
class Animation
{
public:
  void Test(void);
  Animation(void);
private:
  std::shared_ptr<class AnimationImpl> impl_;
};
}
#endif