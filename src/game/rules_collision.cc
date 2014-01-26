#include "rules_collision.h"
#include "collision_group.h"

namespace game
{
class RulesCollisionImpl final : public CollisionGroup<RulesCollision::Channel>
{
  using CollisionGroup::CollisionGroup;
};

void RulesCollision::Add(int group, display::BoundingBox const& bounding_box, Channel const& channel)
{
  impl_->Add(group, bounding_box, channel);
}

void RulesCollision::Link(int group_a, int group_b)
{
  impl_->Link(group_a, group_b);
}

RulesCollision::RulesCollision(Collision const& collision)
{
  impl_ = std::make_shared<RulesCollisionImpl>(collision);
}
}