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
  void Add(int this_group, int other_group, display::BoundingBox const& bounding_box, event::Command const& command);
  void Check(void);

  ~Collision(void);
  Collision(Collision const& other);
  Collision(Collision&& other);
  Collision& operator=(Collision other);
private:
  std::shared_ptr<class CollisionImpl> impl_;
};
}
#endif