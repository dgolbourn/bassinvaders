#ifndef RULES_COLLISION_H_
#define RULES_COLLISION_H_
#include <memory>
#include "collision.h"
#include "channel.h"
namespace game
{
class RulesCollision
{
public:
  RulesCollision(void);
  RulesCollision(Collision const& collision);
  typedef std::pair<int, int> Rules;
  typedef event::Channel<Rules> Channel;
  void Add(int group, display::BoundingBox const& bounding_box, Channel::Pair const& channel);
  void Link(int group_a, int group_b);

  ~RulesCollision(void);
  RulesCollision(RulesCollision const& other);
  RulesCollision(RulesCollision&& other);
  RulesCollision& operator=(RulesCollision other);
private:
  std::shared_ptr<class RulesCollisionImpl> impl_;
};
}
#endif