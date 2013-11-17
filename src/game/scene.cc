#include "scene.h"
#include <map>

namespace game
{
typedef std::weak_ptr<event::CommandImpl> RenderCommandPtr;
typedef std::multimap<int, RenderCommandPtr> RenderCommandMap;
typedef std::pair<int, RenderCommandPtr> RenderCommandPair;

class SceneImpl
{
public:
  void Add(event::Command const& render, int z);
  void Render(void);
  RenderCommandMap commands_;
};

void SceneImpl::Render(void)
{
  for(auto iter = commands_.begin(); iter != commands_.end();)
  {
    if(auto command = iter->second.lock())
    {
      command->operator()();
      ++iter;
    }
    else
    {
      iter = commands_.erase(iter);
    }
  }
}

void SceneImpl::Add(event::Command const& command, int z)
{
  commands_.insert(RenderCommandPair(z, command));
}

Scene::Scene(void) : impl_(new SceneImpl())
{
}

void Scene::Render(void)
{
  impl_->Render();
}

void Scene::Add(event::Command const& command, int z)
{
  impl_->Add(command, z);
}

Scene::Scene(Scene const& other) : impl_(other.impl_)
{
}

Scene::Scene(Scene&& other) : impl_(std::move(other.impl_))
{
}

Scene::~Scene(void)
{
}

Scene& Scene::operator=(Scene other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}