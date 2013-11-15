#include "collision.h"
#include <list>
#include <map>

namespace game
{
typedef std::pair<display::BoundingBox, display::BoundingBox> CollisionPair;
typedef std::weak_ptr<event::CommandImpl> CommandPtr;
typedef std::list<CommandPtr> CommandList;
typedef std::map<CollisionPair, CommandList> CollisionMap;

class CollisionImpl
{
public:
  void Check(void);
  void Add(display::BoundingBox const& a, display::BoundingBox const& b, event::Command const& c);
  CollisionMap commands_;
};

void CollisionImpl::Check(void)
{
  for(auto iter = commands_.begin(); iter != commands_.end();)
  {
    bool erase_flag = true;
    if(display::BoundingBox first_region = iter->first.first)
    {
      if(display::BoundingBox second_region = iter->first.second)
      {
        erase_flag = false;
        if(first_region && second_region)
        {
          for(auto command_iter = iter->second.begin(); command_iter != iter->second.end();)
          {
            if(event::Command command = command_iter->lock())
            {
              command->operator()();
              ++command_iter;
            }
            else
            {
              command_iter = iter->second.erase(command_iter);
            }
          }
          if(iter->second.size() == 0)
          {
            erase_flag = true;
          }
        }
      }
    }
    if(erase_flag)
    {
      iter = commands_.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
}

void CollisionImpl::Add(display::BoundingBox const& a, display::BoundingBox const& b, event::Command const& c)
{
  CollisionPair pair;
  if(a < b)
  {
    pair.first = a;
    pair.second = b;
  }
  else
  {
    pair.first = b;
    pair.second = a;
  }
  commands_[pair].push_back(c);
}

Collision::Collision(void)
{
  impl_ = std::shared_ptr<CollisionImpl>(new CollisionImpl());
}

void Collision::Add(display::BoundingBox const& a, display::BoundingBox const& b, event::Command const& c)
{
  impl_->Add(a, b, c);
}

void Collision::Check(void)
{
  impl_->Check();
}

Collision::Collision(Collision const& other) : impl_(other.impl_)
{
}

Collision::Collision(Collision&& other) : impl_(std::move(other.impl_))
{
}

Collision::~Collision(void)
{
}

Collision& Collision::operator=(Collision other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}