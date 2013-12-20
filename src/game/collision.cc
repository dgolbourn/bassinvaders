#include "collision.h"
#include <list>
#include <map>
#include <iostream>

namespace game
{
typedef std::pair<display::BoundingBox::WeakPtr, display::BoundingBox::WeakPtr> BoundingBoxPair;
typedef std::list<event::Command> CommandList;
typedef std::list<display::BoundingBox::WeakPtr> MemberList;
typedef std::pair<display::BoundingBox::WeakPtr, event::Command> CollisionPair;
typedef std::list<CollisionPair> CollisionPairList;

typedef std::map<BoundingBoxPair, CommandList> CollisionMap;
typedef std::map<int, MemberList> MemberMap;
typedef std::map<int, CollisionPairList> TargetMap;

class CollisionImpl
{
public:
  void Check(void);
  void Add(display::BoundingBox const& a, display::BoundingBox const& b, event::Command const& c);
  void Add(int this_group, int other_group, display::BoundingBox const& bounding_box, event::Command const& command);
  CollisionMap collisions_;
  MemberMap members_;
  TargetMap targets_;
};

void CollisionImpl::Add(int this_group, int other_group, display::BoundingBox const& bounding_box, event::Command const& command)
{
  for(auto iter = targets_[this_group].begin(); iter != targets_[this_group].end();)
  {
    if(display::BoundingBox other_box = iter->first.Lock())
    {
      Add(bounding_box, other_box, iter->second);
      ++iter;
    }
    else
    {
      iter = targets_[this_group].erase(iter);
    }
  }

  for(auto iter = members_[other_group].begin(); iter != members_[other_group].end();)
  {
    if(display::BoundingBox other_box = iter->Lock())
    {
      Add(bounding_box, other_box, command);
      ++iter;
    }
    else
    {
      iter = members_[other_group].erase(iter);
    }
  }

  members_[this_group].push_back(bounding_box);
  targets_[other_group].push_back(CollisionPair(bounding_box, command));
}

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
          if(iter->second.size() == 0)
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
  BoundingBoxPair pair;
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
  collisions_[pair].push_back(c);
}

Collision::Collision(void) : impl_(new CollisionImpl())
{
}

void Collision::Add(display::BoundingBox const& a, display::BoundingBox const& b, event::Command const& c)
{
  impl_->Add(a, b, c);
}

void Collision::Add(int this_group, int other_group, display::BoundingBox const& bounding_box, event::Command const& command)
{
  impl_->Add(this_group, other_group, bounding_box, command);
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