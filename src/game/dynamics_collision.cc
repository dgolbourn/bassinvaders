#include "dynamics_collision.h"
#include "collision_group.h"
namespace game
{
typedef event::Send<Dynamics> Send;
typedef event::Receive<Dynamics> Receive;
typedef event::Channel<Dynamics> Channel;

class DynamicsCollisionImpl final : public CollisionGroup<Channel>
{
  using CollisionGroup::CollisionGroup;
};

void DynamicsCollision::Add(int group, Dynamics const& dynamics, display::BoundingBox const& bounding_box)
{
  Dynamics::WeakPtr ptr = dynamics;
  Send send = [=](void)
  {
    Dynamics shared = ptr.Lock();
    return std::pair<Dynamics, bool>(shared, bool(shared));
  };

  Receive receive = [=](Dynamics const& other)
  {
    bool locked = false;
    if(auto shared = ptr.Lock())
    {
      shared.Collision(other);
      locked = true;
    }
    return locked;
  };

  impl_->Add(group, bounding_box, Channel(send, receive));
}

void DynamicsCollision::Link(int group_a, int group_b)
{
  impl_->Link(group_a, group_b);
}

DynamicsCollision::DynamicsCollision(Collision const& collision)
{
  impl_ = std::make_shared<DynamicsCollisionImpl>(collision);
}
}