#ifndef COLLISION_H_
#define COLLISION_H_

#include <memory>
#include "command.h"
#include "bounding_box.h"

namespace game
{
class Collision
{
public:
  Collision(void);
  void Add(display::BoundingBox const& a, display::BoundingBox const& b, event::Command const& c);
  void Check(void);
private:
  std::shared_ptr<class CollisionImpl> impl_;
};
}
#endif