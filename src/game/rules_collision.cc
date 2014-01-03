#include "rules_collision.h"
#include <list>
#include <map>
#include <set>
namespace game
{
typedef std::pair<display::BoundingBox::WeakPtr, RulesCollision::Channel::Pair> Member;
typedef std::list<Member> MemberList;
typedef std::map<int, MemberList> MemberMap;
typedef std::set<int> LinkSet;
typedef std::map<int, LinkSet> LinkMap;

class RulesCollisionImpl
{
public:
  RulesCollisionImpl(Collision const& collision);
  void Add(int this_group, display::BoundingBox const& bounding_box, RulesCollision::Channel::Pair const& channel);
  void Link(int group_a, int group_b);
  Collision collision_;
  MemberMap members_;
  LinkMap links_;
};

void RulesCollisionImpl::Link(int group_a, int group_b)
{
  if(group_a > group_b)
  {
    std::swap(group_a, group_b);
  }
  if(links_[group_a].insert(group_b).second)
  {
    (void)links_[group_b].insert(group_a);
    MemberList& members_a = members_[group_a];
    MemberList& members_b = members_[group_b];
    for(auto iter_a = members_a.begin(); iter_a != members_a.end();)
    {
      if(display::BoundingBox box_a = iter_a->first.Lock())
      {
        for(auto iter_b = members_b.begin(); iter_b != members_b.end();)
        {
          if(display::BoundingBox box_b = iter_b->first.Lock())
          {
            collision_.Add(box_a, box_b, event::Bind(iter_a->second, iter_b->second));
            ++iter_b;
          }
          else
          {
            iter_b = members_b.erase(iter_b);
          }
        }
        ++iter_a;
      }
      else
      {
        iter_a = members_a.erase(iter_a);
      }
    }
  }
}

void RulesCollisionImpl::Add(int this_group, display::BoundingBox const& this_box, RulesCollision::Channel::Pair const& channel)
{
  for(int group : links_[this_group])
  {
    MemberList& members = members_[group];
    for(auto iter = members.begin(); iter != members.end();)
    {
      if(display::BoundingBox box = iter->first.Lock())
      {
        collision_.Add(this_box, box, event::Bind(iter->second, channel));
        ++iter;
      }
      else
      {
        iter = members.erase(iter);
      }
    }
  }
  members_[this_group].push_back(Member(this_box, channel));
}

RulesCollisionImpl::RulesCollisionImpl(Collision const& collision) : collision_(collision)
{
}

void RulesCollision::Add(int group, display::BoundingBox const& bounding_box, RulesCollision::Channel::Pair const& channel)
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

RulesCollision::RulesCollision(void)
{
}

RulesCollision::RulesCollision(RulesCollision const& other) : impl_(other.impl_)
{
}

RulesCollision::RulesCollision(RulesCollision&& other) : impl_(std::move(other.impl_))
{
}

RulesCollision::~RulesCollision(void)
{
}

RulesCollision& RulesCollision::operator=(RulesCollision other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}