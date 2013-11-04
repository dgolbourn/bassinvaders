#include "scene.h"
#include <map>

namespace game
{
typedef std::multimap<int, std::weak_ptr<RenderCommandImpl>> RenderCommandMap;

class SceneImpl
{
public:
  void Subscribe(RenderCommand const& render, int z);
  void Render(void);

  RenderCommandMap renders_;
};

void SceneImpl::Render(void)
{
  for(auto iter = renders_.begin(); iter != renders_.end(); ++iter)
  {
    auto render = iter->second.lock();
    if(render)
    {
      render->operator()();
    }
    else
    {
      renders_.erase(iter);
    }
  }
}

void SceneImpl::Subscribe(RenderCommand const& render, int z)
{
  renders_.insert(std::pair<int, std::weak_ptr<RenderCommandImpl>>(z, render));
}

Scene::Scene(void)
{
  impl_ = std::shared_ptr<SceneImpl>(new SceneImpl);
}

void Scene::Render(void)
{
  impl_->Render();
}

void Scene::Subscribe(RenderCommand const& render, int z)
{
  impl_->Subscribe(render, z);
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