#include "collision.h"
#include <list>
#include <map>

namespace game
{
typedef std::pair<display::BoundingBox::WeakPtr, display::BoundingBox::WeakPtr> BoundingBoxPair;
typedef std::list<event::Command> CommandList;
typedef std::map<BoundingBoxPair, CommandList> CollisionMap;

class CollisionImpl
{
public:
  void Check(void);
  void Add(display::BoundingBox const& a, display::BoundingBox const& b, event::Command const& c);
  CollisionMap collisions_;
};

void CollisionImpl::Check(void)
{
  for(auto iter = collisions_.begin(); iter != collisions_.end();)
  {
    bool erase_flag = true;
    if(display::BoundingBox first_region = iter->first.first.Lock())
    {
      if(display::BoundingBox second_region = iter->first.second.Lock())
      {
        erase_flag = false;
        if(first_region && second_region)
        {
          for(auto command_iter = iter->second.begin(); command_iter != iter->second.end();)
          {
            if((*command_iter)())
            {
              ++command_iter;
            }
            else
            {
              command_iter = iter->second.erase(command_iter);
            }
          }
          if(iter->second.empty())
          {
            erase_flag = true;
          }
        }
      }
    }
    if(erase_flag)
    {
      iter = collisions_.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
}

void CollisionImpl::Add(display::BoundingBox const& a, display::BoundingBox const& b, event::Command const& c)
{
  if(a > b)
  {
    collisions_[BoundingBoxPair(b, a)].push_back(c);
  }
  else
  {
    collisions_[BoundingBoxPair(a, b)].push_back(c);
  }
}

Collision::Collision(void)
{
  impl_ = std::make_shared<CollisionImpl>();
}

void Collision::Add(display::BoundingBox const& a, display::BoundingBox const& b, event::Command const& c)
{
  impl_->Add(a, b, c);
}

void Collision::Check(void)
{
  impl_->Check();
}
}